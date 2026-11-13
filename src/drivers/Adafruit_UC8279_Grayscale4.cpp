#include "Adafruit_UC8279_Grayscale4.h"

#include "Adafruit_EPD.h"

// clang-format off

// Four-tone waveform from the MIT-licensed Free-Ink SDK's makeQualityBank()
// (Uc8279X4Driver.cpp) at FREEINK_UC8279X4_GRAY_SPEED 100: the X3 XTH4
// four-grey tables, rebalanced to zero net charge and padded to equal length.
// Rows are in upload order: the source's table 2 goes to 0x23 and table 3 to
// 0x22. Trailing bytes are zero.
static const uint8_t uc8279_gray_lut_4tone[5][50] = {
    {UC8279_LUT_VCOM,
     0x01, 0x10, 0x06, 0x02, 0x03, 0x01, 0x01,
     0x01, 0x01, 0x01, 0x02, 0x02, 0x01, 0x01,
     0x01, 0x14, 0x03, 0x02, 0x03, 0x01, 0x01,
     0x01, 0x01, 0x01, 0x01, 0x07, 0x01, 0x01},
    {UC8279_LUT_WW,
     0x01, 0x90, 0x86, 0x02, 0x03, 0x01, 0x01,
     0x01, 0x01, 0x01, 0x02, 0x02, 0x01, 0x01,
     0x01, 0x54, 0x83, 0x02, 0x83, 0x01, 0x01,
     0x01, 0x81, 0x41, 0x41, 0x47, 0x01, 0x01},
    {UC8279_LUT_WB,
     0x01, 0x90, 0x06, 0x82, 0x83, 0x01, 0x01,
     0x01, 0x01, 0x41, 0x42, 0x42, 0x01, 0x01,
     0x01, 0x54, 0x83, 0x42, 0x83, 0x01, 0x01,
     0x01, 0x01, 0x01, 0x01, 0x07, 0x01, 0x01},
    {UC8279_LUT_BW,
     0x01, 0x90, 0x06, 0x82, 0x83, 0x01, 0x01,
     0x01, 0x81, 0x41, 0x42, 0x42, 0x01, 0x01,
     0x01, 0x54, 0x83, 0x02, 0x03, 0x01, 0x01,
     0x01, 0x01, 0x01, 0x01, 0x07, 0x01, 0x01},
    {UC8279_LUT_BB,
     0x01, 0x10, 0x06, 0x82, 0x83, 0x01, 0x01,
     0x01, 0x81, 0x01, 0x42, 0x42, 0x01, 0x01,
     0x01, 0x54, 0x83, 0x82, 0x83, 0x01, 0x01,
     0x01, 0x81, 0x81, 0x81, 0x87, 0x01, 0x01},
};

// clang-format on

/**************************************************************************/
/*!
    @brief constructor if using external SRAM chip and software SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param SID the SID pin to use
    @param SCLK the SCLK pin to use
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param SRCS the SRAM chip select pin to use
    @param MISO the MISO pin to use
    @param BUSY the busy pin to use
*/
/**************************************************************************/
Adafruit_UC8279_Grayscale4::Adafruit_UC8279_Grayscale4(
    int width, int height, int16_t SID, int16_t SCLK, int16_t DC, int16_t RST,
    int16_t CS, int16_t SRCS, int16_t MISO, int16_t BUSY)
    : Adafruit_UC8279(width, height, SID, SCLK, DC, RST, CS, SRCS, MISO, BUSY) {
  // The base constructor sized buffer1 only; grayscale needs both planes.
  buffer2_size = buffer1_size;
  if (use_sram) {
    buffer2_addr = buffer1_size;
  } else {
    buffer2 = (uint8_t*)malloc(buffer2_size);
  }
}

/**************************************************************************/
/*!
    @brief constructor if using on-chip RAM and hardware SPI
    @param width the width of the display in pixels
    @param height the height of the display in pixels
    @param DC the data/command pin to use
    @param RST the reset pin to use
    @param CS the chip select pin to use
    @param SRCS the SRAM chip select pin to use
    @param BUSY the busy pin to use
    @param spi the SPI bus to use
*/
/**************************************************************************/
Adafruit_UC8279_Grayscale4::Adafruit_UC8279_Grayscale4(int width, int height,
                                                       int16_t DC, int16_t RST,
                                                       int16_t CS, int16_t SRCS,
                                                       int16_t BUSY,
                                                       SPIClass* spi)
    : Adafruit_UC8279(width, height, DC, RST, CS, SRCS, BUSY, spi) {
  // The base constructor sized buffer1 only; grayscale needs both planes.
  buffer2_size = buffer1_size;
  if (use_sram) {
    buffer2_addr = buffer1_size;
  } else {
    buffer2 = (uint8_t*)malloc(buffer2_size);
  }
}

/**************************************************************************/
/*!
    @brief begin communication with and set up the display.
    @param reset if true the reset pin will be toggled.
*/
/**************************************************************************/
void Adafruit_UC8279_Grayscale4::begin(bool reset) {
  Adafruit_UC8279::begin(reset);

  // Absolute plane encoding (see class docs): layer bit 0 -> DTM1, bit 1 ->
  // DTM2, written straight through. Cleared buffers = (0,0) = white.
  setBlackBuffer(0, false);
  setColorBuffer(1, false);

  layer_colors[EPD_WHITE] = 0b00;
  layer_colors[EPD_BLACK] = 0b11;
  layer_colors[EPD_RED] = 0b01;
  layer_colors[EPD_GRAY] = 0b10;
  layer_colors[EPD_LIGHT] = 0b01;
  layer_colors[EPD_DARK] = 0b10;

  // powerUp() pre-fills both planes across the addressed scan; (0,0) is
  // white in this encoding, so the unbonded gates stay white too.
  _blank_fill = 0x00;
}

/**************************************************************************/
/*!
    @brief signal the display to update
*/
/**************************************************************************/
void Adafruit_UC8279_Grayscale4::update() {
  // Register LUTs only take effect after an OTP (REG=0) refresh in the same
  // power session. An OTP refresh of a small window in the unbonded gates
  // arms them without touching the visible image.
  EPD_command(UC8279_VCOM_DATA_INTERVAL, &_cdi, 1);
  uint8_t v = _ccset;
  EPD_command(UC8279_CCSET, &v, 1);
  v = _tsset;
  EPD_command(UC8279_TSSET, &v, 1);

  EPD_command(UC8279_POWERON);
  busy_wait();

  // Arm: OTP refresh of an 8 x 10 window at the end of the addressed scan,
  // which is past the 480 bonded gates.
  const uint16_t y0 = _addressed_gates - 10;
  const uint16_t y1 = _addressed_gates - 1;
  const uint8_t ptl[9] = {0x00,
                          0x00,
                          0x00,
                          0x07,
                          (uint8_t)(y0 >> 8),
                          (uint8_t)(y0 & 0xFF),
                          (uint8_t)(y1 >> 8),
                          (uint8_t)(y1 & 0xFF),
                          0x01};
  uint8_t psr[2] = {(uint8_t)(_psr0 & 0xDF), _psr1}; // REG=0: OTP waveform
  EPD_command(UC8279_PARTIAL_IN);
  EPD_command(UC8279_PARTIAL_WINDOW, ptl, 9);
  EPD_command(UC8279_PANELSETTING, psr, 2);
  EPD_command(UC8279_DISPLAYREFRESH);
  delay(10);
  busy_wait();
  EPD_command(UC8279_PARTIAL_OUT);

  // Gray pass (Free-Ink SDK displayGray order, panel already on):
  // PSR REG=1 -> LUTs -> CDI -> PSR -> DRF.
  psr[0] = _psr0;
  EPD_command(UC8279_PANELSETTING, psr, 2);
  for (uint8_t t = 0; t < 5; t++) {
    EPD_command(uc8279_gray_lut_4tone[t][0], &uc8279_gray_lut_4tone[t][1],
                sizeof(uc8279_gray_lut_4tone[t]) - 1);
  }
  EPD_command(UC8279_VCOM_DATA_INTERVAL, &_cdi, 1);
  EPD_command(UC8279_PANELSETTING, psr, 2);

  EPD_command(UC8279_DISPLAYREFRESH);
  delay(100);
  busy_wait();

  if (_busy_pin <= -1) {
    delay(default_refresh_delay);
  }
}

/**************************************************************************/
/*!
    @brief Send the specific command to start writing to EPD display RAM
    @param index 0 writes the DTM1 plane (buffer1), 1 the DTM2 plane
   (buffer2); together they select the per-pixel gray level.
    @returns The byte that is read from SPI at the same time as sending the
   command
*/
/**************************************************************************/
uint8_t Adafruit_UC8279_Grayscale4::writeRAMCommand(uint8_t index) {
  return EPD_command(index == 0 ? UC8279_WRITE_RAM1 : UC8279_WRITE_RAM2, false);
}
