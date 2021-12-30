#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "oled.h"

static SSD1306AsciiWire oled;

void Oled::init() {
	Wire.begin();
  Wire.setClock(400000L);
  oled.begin(&Adafruit128x64, 0x3C);
  oled.setFont(System5x7);
  oled.clear();
}

void Oled::handle() {
	static bool prevHeaterState = false;
	if (Machine::getHeaterState() != prevHeaterState) {
		prevHeaterState = Machine::getHeaterState();
		oled.setCursor(20, 0);
		oled.set2X();
		oled.print(prevHeaterState ? '*' : '_');
		oled.set1X();
	}

	static int prevTemp = -1;
	if (Machine::getTemp() != prevHeaterState) {
		prevTemp = Machine::getTemp();
		oled.setCursor(40, 0);
		oled.set2X();
		oled.print(prevTemp);
		oled.print("'C");
		oled.clearToEOL();
		oled.set1X();
	}

	static IPAddress prevIp = -1;
	if (Wifi::getIp() != prevIp) {
		prevIp = Wifi::getIp();
		oled.setCursor(0, 2);
		oled.print("IP: ");
		oled.print(Wifi::getIp().toString());
		oled.clearToEOL();
	}

	static uint32_t prevStickyTime = -1;
	static bool prevRecSticky = true;;
	if (prevStickyTime != config->stickyTime || prevRecSticky != Machine::recordStickyTime) {
		prevStickyTime = config->stickyTime;
		prevRecSticky = Machine::recordStickyTime;

		oled.setCursor(0, 3);
		oled.print(F("StickyT: "));
		if (prevRecSticky) oled.print(F("REC"));
		else {
			oled.print(prevStickyTime / 1000.0, 3);
			oled.print(" s");
		}
		oled.clearToEOL();
	}

	static uint8_t prevAutoRunner = -1;
	static uint32_t prevAutoRunnerInterval = -1;
	static uint32_t prevAutoRunnerDuration = -1;
	if (prevAutoRunner != Machine::autoRunner ||
		prevAutoRunnerInterval != config->autoRunnerInterval ||
		prevAutoRunnerDuration != config-> autoRunnerDuration
	) {
		prevAutoRunner = Machine::autoRunner;
		prevAutoRunnerInterval = config->autoRunnerInterval;
		prevAutoRunnerDuration = config-> autoRunnerDuration;

		oled.setCursor(0, 4);
		oled.print("A: ");
		oled.print(prevAutoRunner ? "ON " : "OFF ");
		oled.print(prevAutoRunnerInterval / 1000.0, 2);
		oled.print("/");
		oled.print(prevAutoRunnerDuration / 1000.0, 3);
		oled.print("s");
		oled.clearToEOL();
	}

	static uint8_t prevRadioLearnCode = 0;
	if (prevRadioLearnCode != Radio::learnCode) {
		prevRadioLearnCode = Radio::learnCode;
		oled.setCursor(0, 5);
		if (prevRadioLearnCode) {
			oled.print(prevRadioLearnCode == 1 ? F("REC radio simple") : F("REC radio sticky"));
		}
		oled.clearToEOL();
	}
}
