# liam-esp
[![GitHub release](https://img.shields.io/github/release/trycoon/liam-esp.svg?maxAge=3600)][GitHub release]
[![Travis](https://img.shields.io/travis/trycoon/liam-esp.svg?maxAge=3600)][Travis]
[![license](https://img.shields.io/github/license/trycoon/liam-esp.svg?maxAge=3600)][license]


## Overview
ESP32 port of Liam Arduino lawn mower


## Features
using plattform.io

TODO: implement first-time portal https://github.com/tzapu/WiFiManager
or https://github.com/copercini/esp32-iot-examples/blob/master/WiFi_portal/WiFi_portal.ino
for ESP32 look at this fork: https://github.com/bbx10/WebServer_tng, https://github.com/bbx10/DNSServer_tng, https://github.com/bbx10/ESPWebServer, https://github.com/bbx10/Hash_tng

TODO: check if we could use a PID-library to tune BWF readings when docking. https://github.com/br3ttb/Arduino-PID-Library

TODO: uptime and time keeping: https://github.com/vitotai/BrewManiacEsp8266/blob/master/BrewManiacEx/TimeKeeper.cpp

TODO: Multithreading on ESP32, processable.h-interface and let each class use this one instead: https://github.com/copercini/esp32-iot-examples/blob/master/multiloop/multiloop.ino, if not working then look at this: https://www.arduino.cc/en/Reference/Scheduler or this (works even in Arduino-mode): http://www.freertos.org/a00125.html

TODO: save and load settings from Preference, save statechanges in state_controller so we can continue after reboot:
https://github.com/espressif/arduino-esp32/blob/master/libraries/Preferences/examples/StartCounter/StartCounter.ino

TODO: see this for inspiration: https://bitbucket.org/xoseperez/espurna/src/e6e9f9d6d4bd16005446e08b98d47c6cd254ad16?at=master

## Build software


## Upload software

platformio run -t upload

[SPIFFS](http://docs.platformio.org/en/latest/platforms/espressif8266.html#platform-espressif-uploadfs)
platformio run -t buildfs
platformio run -t uploadfs --upload-port 192.168.10.73


## License & Author


- Author:: Henrik Östman

```
MIT License

Copyright (c) 2017 Henrik Östman

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
```
