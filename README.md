# liam-esp
[![GitHub release](https://img.shields.io/github/release/trycoon/liam-esp.svg?maxAge=3600)][GitHub release]
[![Travis](https://img.shields.io/travis/trycoon/liam-esp.svg?maxAge=3600)][Travis]
[![license](https://img.shields.io/github/license/trycoon/liam-esp.svg?maxAge=3600)][license]


## Overview
ESP8266 port of Liam Arduino lawn mower


## Features
using plattform.io

TODO: implement first-time portal https://github.com/tzapu/WiFiManager
TODO: check if we could use a PID-library to tune BWF readings when docking. https://github.com/br3ttb/Arduino-PID-Library
TODO: uptime and time keeping: https://github.com/vitotai/BrewManiacEsp8266/blob/master/BrewManiacEx/TimeKeeper.cpp
TODO: see this for inspiration: https://bitbucket.org/xoseperez/espurna/src/e6e9f9d6d4bd16005446e08b98d47c6cd254ad16?at=master

## Build software


## Upload software

platformio run -t upload

[SPIFFS](http://docs.platformio.org/en/latest/platforms/espressif8266.html#platform-espressif-uploadfs)
platformio run -t buildfs
platformio run -t uploadfs --upload-port 192.168.10.101


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
