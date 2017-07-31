#ifndef io_digital_h
#define io_digital_h

#include <I2cDiscreteIoExpander.h>
// For PCF8575 (don't use PCF8575C since its 5 volt), use https://github.com/4-20ma/I2cDiscreteIoExpander
// For MCP23008, use https://github.com/adafruit/Adafruit-MCP23008-library

class IO_Digital {
  public:
    IO_Digital();
    bool digitalWrite(uint8_t pin, bool state);

  private:
    I2cDiscreteIoExpander device;
};

#endif
