/***************************************************
  4-level grayscale test for the Xteink X4 Pro Pocket eReader (UC8279,
  800x480), running on the device's own ESP32-S3.

  Draws four labeled vertical bars, one per gray level, so a single photo
  shows all four: EPD_WHITE, EPD_LIGHT, EPD_DARK, EPD_BLACK.

  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include "Adafruit_EPD.h"

// Xteink X4 Pro GPIOs (same as the Adafruit Marquee X4 Pro preset)
#define EPD_SCK 12
#define EPD_MOSI 11
#define EPD_CS 13
#define EPD_DC 18
#define EPD_RESET 14
#define EPD_BUSY 6 // LOW while the panel is busy
#define SRAM_CS -1 // no external SRAM, the ESP32-S3 holds both planes

#define X4_PERIPH_POWER 1 // HIGH turns on the peripheral rail
#define X4_SD_POWER_N 5   // HIGH turns the SD card off

Adafruit_UC8279_Grayscale4 display(800, 480, EPD_DC, EPD_RESET, EPD_CS, SRAM_CS,
                                   EPD_BUSY, &SPI);

static void drawBar(int16_t i, uint16_t color, const char* label) {
  const int16_t w = display.width() / 4;
  display.fillRect(i * w, 60, w, display.height() - 60, color);
  display.setCursor(i * w + 10, 20);
  display.setTextColor(EPD_BLACK);
  display.print(label);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("X4 Pro gray4 test");

  pinMode(X4_PERIPH_POWER, OUTPUT);
  digitalWrite(X4_PERIPH_POWER, HIGH);
  delay(50);
  pinMode(X4_SD_POWER_N, OUTPUT);
  digitalWrite(X4_SD_POWER_N, HIGH);

#if defined(ARDUINO_ARCH_ESP32)
  SPI.begin(EPD_SCK, -1, EPD_MOSI, EPD_CS);
#endif

  display.begin();
  display.setTextSize(3);
  display.setTextWrap(false);

  display.clearBuffer();
  drawBar(0, EPD_WHITE, "WHITE");
  drawBar(1, EPD_LIGHT, "LIGHT");
  drawBar(2, EPD_DARK, "DARK");
  drawBar(3, EPD_BLACK, "BLACK");

  const uint32_t t0 = millis();
  display.display();
  Serial.print("display() took ");
  Serial.print(millis() - t0);
  Serial.println(" ms");
}

void loop() {
  delay(1000);
}
