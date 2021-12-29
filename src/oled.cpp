#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include "oled.h"

static const int OLED_REFRESH_TIME = 500;

// 'heater', 9x8px
const unsigned char icon_heater [] PROGMEM = {
	0x40, 0x66, 0x59, 0x40, 0x66, 0x59, 0x40, 0x66, 0x59
};

// 'heater_off', 9x8px
const unsigned char icon_heater_off [] PROGMEM = {
	0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40
};

static SSD1306AsciiWire oled;

void Oled::init() {
  oled.begin(&Adafruit128x64, 0x3C);
  oled.setFont(System5x7);
  oled.clear();
}

static void printIcon(const unsigned char* icon, uint8_t x2 = false) {
	for(uint8_t i = 0; i < 9; i++) {
		oled.ssd1306WriteRamBuf(icon[i]);
		if (x2) oled.ssd1306WriteRamBuf(icon[i]);
	}
	oled.ssd1306WriteRam(0x00);
	if (x2 == 1) {
		oled.setRow(oled.row() + 1);
		printIcon(icon, 2);
	}
}

void Oled::handle() {
	static bool prevHeaterState = false;
	if (Machine::getHeaterState() != prevHeaterState) {
		prevHeaterState = Machine::getHeaterState();
		oled.setCursor(0, 0);
		printIcon(prevHeaterState ? icon_heater : icon_heater_off, 1);

	}

	static int prevTemp = 0;
	if (Machine::getTemp() != prevHeaterState) {
		prevTemp = Machine::getTemp();
		oled.setCursor(10, 0);
		oled.set2X();
		oled.print(prevTemp);
		oled.print("\7fC");
		oled.clearToEOL();
		oled.set1X();
	}

	static IPAddress prevIp = 0;
	if (Wifi::getIp() != prevIp) {
		prevIp = Wifi::getIp();
		oled.setCursor(0, 3);
		oled.print("IP: ");
		oled.print(Wifi::getIp().toString());
		oled.clearToEOL();
	}

	static uint32_t prevStickyTime = 0;
	static bool prevRecSticky = false;
	if (prevStickyTime != config->stickyTime || prevRecSticky != Machine::recordStickyTime) {
		prevStickyTime = config->stickyTime;
		prevRecSticky = Machine::recordStickyTime;

		oled.setCursor(0, 4);
		oled.print(F("StickyT: "));
		if (prevRecSticky) oled.print(F("REC"));
		else {
			oled.print(prevStickyTime / 1000.0, 3);
			oled.print(" s");
		}
		oled.clearToEOL();
	}

	static uint8_t prevRadioLearnCode = 0;
	if (prevRadioLearnCode != Radio::learnCode) {
		prevRadioLearnCode = Radio::learnCode;
		oled.setCursor(0, 5);
		if (prevRadioLearnCode) {
			oled.print(prevRadioLearnCode == 1 ? F("REC radio simple") : F("REC radio sticky"));
			oled.clearToEOL();
		}
	}
}
