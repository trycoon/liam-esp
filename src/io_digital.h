#ifndef io_digital_h
#define io_digital_h

class IO_Digital {
  public:
    IO_Digital();

    // For PCF8575 (don't use PCF8575C since its 5 volt), use https://github.com/4-20ma/I2cDiscreteIoExpander
    // For MCP23008, use https://github.com/adafruit/Adafruit-MCP23008-library
};

#endif
