# Liam-ESP

[![license](https://img.shields.io/github/license/trycoon/liam-esp.svg?maxAge=3600)](https://opensource.org/licenses/MIT)

## Introduction

_A project for building your own robotic lawn mower._

The concept and basic ideas are sprung from the ["Liam lawn mower"](https://github.com/sm6yvr/liam) and [ArduMower](https://www.ardumower.de/index.php/en/) projects, but instead of using an Arduino for controlling the mower this project makes use of the more powerful "Espressif ESP32" microcontroller. The ESP32 boosts more CPU power and memory than a average Arduino, leaving us with less limits but still at a low price.

Here are some of the features of this project:

- Automatic mowing of lawn using a two wheeled robot.
- GNSS-RTK navigation support, for accurate mowing and path planning.
- WiFi connected docking station that relays communication to mower over a LoRa-connection.
- Easy to use REST-based API with Swagger support, making it a breeze to integrate with Home automation softwares.
- Monitor mower using [MQTT protocol](documentation/api.md) (works with most Home automation systems).
- Realtime monitoring using Websocket.
- Built in support for scheduled mowing (time of day, days of week).
- Obstacle avoidance using ultrasound transducers.
- Support accelerometer, gyroscope, and e-compass to keep mower on a straight line, and detect if mower has flipped over (safety reasons).
- Integrated Web UI in docking station for controlling and monitoring mower, see [demo site](http://liam.smart-home.rocks/). (username: admin, password: liam)

**This repository only contains hardware schematics and firmware for the mower, the [docking station has its own project](https://github.com/trycoon/liam-dockingstation)!**

### -> [Building the mower body](hardware/3dprint/README.md)

### -> [Building the electronics](hardware/README.md)

### -> [Building the firmware](firmware.md)

## License & Author

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
