#include "max6675.h"
#include "machine.h"

static const int READ_TEMP_INTERVAL = 1000;
static const int WEB_TRIGGER_TIME   = 2000;
static const int RADIO_TRIGGER_TIME = 200;

static const int MIN_TEMP           = 230;
static const int GOAL_TEMP          = 280;

/**
 * Pins
**/
static const int THERMO_SO          = D6;
static const int THERMO_CS          = D8;
static const int THERMO_SCK         = D5;

static const int HEATER_PIN         = D3;
static const int PUMP_PIN           = D4;
static const int RADIO_PIN          = D7;

static MAX6675 thermocouple(THERMO_SCK, THERMO_CS, THERMO_SO);

static uint32_t timer = 0;
static uint32_t startTime = 0;
static int      temp = 0;

void Machine::init() {
  pinMode(HEATER_PIN, OUTPUT);
  pinMode(PUMP_PIN,   OUTPUT);
}

void Machine::handle() {
  uint32_t mil = millis();

  static uint32_t tempTimer = 0;
  if (mil - tempTimer > READ_TEMP_INTERVAL) {
    tempTimer = mil;
    temp = thermocouple.readCelsius();
  }

  static uint32_t autoRunerIntervalTimer = 0;
  if (autoRunner) {
    if (mil - autoRunerIntervalTimer > config->autoRunnerInterval) {
      autoRunerIntervalTimer = mil;
      run(config->autoRunnerDuration);
    }
  } else {
    autoRunerIntervalTimer = 0;
  }

  bool shouldRun = timer && mil < timer;
  static bool lastShouldRun = false;

  if (recordStickyTime && shouldRun != lastShouldRun) {
    if (shouldRun) {
      startTime = mil;
    } else {
      recordStickyTime = 0;
      config->stickyTime = millis() - startTime;
      config.save();
    }
  }

  digitalWrite(PUMP_PIN, !(shouldRun && temp > MIN_TEMP));
  digitalWrite(HEATER_PIN, !(shouldRun || temp < GOAL_TEMP));

  lastShouldRun = shouldRun;
}

void Machine::run(uint16_t time) {
  timer = millis() + time;
}

int Machine::getTemp() {
  return temp;
}

bool Machine::getHeaterState() {
  return !digitalRead(HEATER_PIN);
}

bool Machine::recordStickyTime = false;
bool Machine::autoRunner = false;
