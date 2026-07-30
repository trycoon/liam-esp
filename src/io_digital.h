#ifndef io_digital_h
#define io_digital_h

#include <MCP23017.h>
#include <Wire.h>

class IO_Digital
{
public:
    IO_Digital(TwoWire& w);
    void setPinMode(uint8_t pin, bool input);
    void digitalWrite(uint8_t pin, bool state);
    bool digitalRead(uint8_t pin);

private:
    TwoWire& _Wire;
    MCP23017 device;
};

#endif