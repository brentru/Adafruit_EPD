#ifndef LIB_ADAFRUIT_UC8279_GRAYSCALE4
#define LIB_ADAFRUIT_UC8279_GRAYSCALE4

#include <Arduino.h>

#include "Adafruit_UC8279.h"

/**************************************************************************/
/*!
    @brief  Class for interfacing with UC8279 EPD drivers in 4-level
            grayscale mode using the external LUT bank (PSR REG=1).

    Plane encoding is absolute, matching the stock Xteink X4 Pro firmware
    (byte tables and sequence via the MIT-licensed Free-Ink SDK,
    Uc8279X4Driver): per pixel, (DTM1, DTM2) bits are
    black = (1,1), dark gray = (0,1), light gray = (1,0), white = (0,0).
    buffer1 streams to DTM1 and buffer2 to DTM2 with no inversion, and
    layer_colors produces this encoding directly; a cleared buffer is white.

    Waveform: a four-tone bank (the X3 XTH4 four-grey tables, DC
    re-balanced, as the Free-Ink SDK builds for absolute grayscale).

    Register LUTs only take effect after an OTP refresh in the same power
    session, so update() first runs an OTP refresh on a small window in the
    unbonded gates (invisible), then the gray refresh.
*/
/**************************************************************************/
class Adafruit_UC8279_Grayscale4 : public Adafruit_UC8279 {
 public:
  Adafruit_UC8279_Grayscale4(int width, int height, int16_t SID, int16_t SCLK,
                             int16_t DC, int16_t RST, int16_t CS, int16_t SRCS,
                             int16_t MISO, int16_t BUSY = -1);
  Adafruit_UC8279_Grayscale4(int width, int height, int16_t DC, int16_t RST,
                             int16_t CS, int16_t SRCS, int16_t BUSY = -1,
                             SPIClass* spi = &SPI);

  void begin(bool reset = true);
  void update(void);

 protected:
  uint8_t writeRAMCommand(uint8_t index);
};

#endif
