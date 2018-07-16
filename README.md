# liam-esp
[![GitHub release](https://img.shields.io/github/release/trycoon/liam-esp.svg?maxAge=3600)][GitHub release]
[![Travis](https://img.shields.io/travis/trycoon/liam-esp.svg?maxAge=3600)][Travis]
[![license](https://img.shields.io/github/license/trycoon/liam-esp.svg?maxAge=3600)][license]


## Overview
Software for controlling a home built robotic lawn mower.
The concept and basic ideas are sprung from the ["Liam lawn mower"](https://github.com/sm6yvr/liam) and [ArduMower](https://www.ardumower.de/index.php/en/) projects, but instead of using an Arduino for controlling the mower this project makes use of the more powerful Espressif ESP32 microcontroller. The reasoning behind this decision is the wish for WiFi connectivity and interconnectivity with existing home automation systems on the market.

Here are some of the features of this project:

- Automatic mowing of lawn using a two wheel robot
- Support for limiting mower movement using a Buried Wire Fence (virtual wall)
- WiFi connected (can operate when signal is weak/lost, will reconnect when signal is available)
- Easy to use REST-based API, self explaining (HATEOAS compatible), with Swagger support.  
- Monitor mower using MQTT protocol (works with most Home automation systems)
- NTP time support, making scheduling of mowing a breeze
- Integrated Web UI for controlling and monitoring mower
- Support accelerometer, gyroscope, and e-compass to keep mower on a straight line, and detect if mower are flipped over (safety reasons)

## Build and upload software

To simplify building this software, with all its dependencies, we use the [Platform.io](https://platformio.org/) open source ecosystem. Make sure you have Platform.io installed on your computer before you proceed.

### Compile and upload the Liam-ESP application

Connect a micro-USB cable between your computer and the ESP32 microcontroller, run the following command in the root folder of this project to compile and upload the software to the ESP32:
```
  platformio run -t upload
```

If your computer is stuck waiting on the following lines:
```
  Uploading .pioenvs/nodemcuv2/firmware.bin
  esptool.py v2.3.1
```
and eventually timing out, then you need to press the "flash"-button on the ESP32 for 2-3 seconds when waiting on those lines to initialize the flashing-process!

### Compile and upload the Web User interface

The Web UI is used for controlling and montitoring your mower, it's also used to setup and configure the mower for the first time use.
The user interface is a single-page web application that is served from a web server on the mower, the web UI communicates with the mower using its public available REST-API. To preserve precious memory the Web UI is stored in a separate part of the Flash memory called the SPIFFS-memory. To build a memory image and upload it to SPIFFS a few other build steps needs to be run in Platform.io. Also to keep the size of the Web UI to a minimum it is "minified" and the different files are bundled togather to form just a few, to do this we use common used web developer tools based upon the [Node.js](https://nodejs.org/en/) environment. Soo to build the Web UI first make sure you have Node.js installed!

Enter the "web"-directory of this application and then run the following in the terminal:
```
  npm install
  npm run build
```
Then exit back to the project root directory and run:
```
  platformio run -t buildfs
  platformio run -t uploadfs
```
(click the flash-button on the microcontroller to allow flashing)

Press the "Reset" button on the ESP32 board and you should now be able to visit the mowers [user interface](http://liam-esp.local) using a webbrowser.

## TODO

TODO: logging library: https://esp-idf.readthedocs.io/en/v1.0/api/log.html
TODO: check if we could use a PID-library to tune BWF readings when docking. https://github.com/br3ttb/Arduino-PID-Library
TODO: Multithreading on ESP32, processable.h-interface and let each class use this one instead: https://github.com/copercini/esp32-iot-examples/blob/master/multiloop/multiloop.ino, if not working then look at this: https://www.arduino.cc/en/Reference/Scheduler or this (works even in Arduino-mode): http://www.freertos.org/a00125.html
TODO: see this for inspiration: https://bitbucket.org/xoseperez/espurna/src

## License & Author


- Author: Henrik Östman

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
