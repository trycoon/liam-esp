#include "Arduino.h"
#include "io_digital.h"
#include "definitions.h"

// Useful MCP23017 information: https://www.best-microcontroller-projects.com/mcp23017.html

IO_Digital::IO_Digital(TwoWire& w): _Wire(w), device(Definitions::DIGITAL_EXPANDER_ADDR, w) {

  device.init();
  device.interruptMode(MCP23017_INTMODE::OR);

}

void IO_Digital::setPinMode(uint8_t pin, bool input) {
  device.pinMode(pin, input);
}

void IO_Digital::digitalWrite(uint8_t pin, bool state) {
/*
https://github.com/blemasle/arduino-mcp23017/blob/master/examples/PortCopyOnInterrupt/PortCopyOnInterrupt.ino
mcp.init();
    mcp.portMode(MCP23017_PORT::A, 0);         //Port A as ouput
    mcp.portMode(MCP23017_PORT::B, 0b11111111);//Port B as input

    mcp.interruptMode(MCP23017_INTMODE::SEPARATED);      
    mcp.interrupt(MCP23017_PORT::B, FALLING);

    mcp.writeRegister(MCP23017_REGISTER::GPIOA, 0x00);
    mcp.writeRegister(MCP23017_REGISTER::GPIOB, 0x00);

    mcp.clearInterrupts();
    attachInterrupt(1, userInput, FALLING);
    */
  device.digitalWrite(pin, state);
}

bool IO_Digital::digitalRead(uint8_t pin) {
  return device.digitalRead(pin);
}

