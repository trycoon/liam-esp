/**
 * Liam-ESP emulator
 * Simple application using [webpack-api-mocker](https://www.npmjs.com/package/webpack-api-mocker) to emulate a Liam-ESP lawnmover and answering the same REST request the mower normally does.
 * This is useful when you want to develop the GUI without having a lawn mover at hand (or risking starting the cutter and chopping of your hands).
 *
 * Start emulator:
 * 1. install Node.js (https://nodejs.org)
 * 2. "npm install" (installs all dependencies)
 * 3. "npm run dev" (start emulator)
 * 4. point webbrowser to "http://localhost:8080"
 */
let currentState = {
      state: 'DOCKED',
      isCharging: false,
      cutterRotating: false,
      batteryVoltage: Math.floor(Math.random() * (10 - 16 + 1)) + 16,
      batteryLevel: 90,
      lastFullyChargeTime: new Date() - 1000,
      lastChargeDuration: 1000 *60 * 60,
      cutterLoad: Math.random() * 100,
      leftWheelSpd: 80,
      rightWheelSpd: 80,
      pitch: 0,
      roll: 0,
      heading: 45,
      targetHeading: 45
    },
    batterySamples = function() {
      let samples = [],
          date = new Date().getTime();

      for (let i = 0; i < 100; i++) {
        samples.push({
          t: date + i,
          v: 14.8 - i / 10
        });
      }

      return samples;
    },
    uptime = new Date(),
    proxy = {
      'GET /api/v1/status': (req, res) => {
        currentState.batteryVoltage = Math.floor(Math.random() * (10 - 16 + 1)) + 16;
        currentState.cutterLoad = Math.random() * 100;
        return res.json(currentState);
      },
      'PUT /api/v1/state': (req, res) => {
        currentState.state = req.body.state;
      },
      'PUT /api/v1/manual/cutter_on': (req, res) => {
        currentState.cutterRotating = true;
      },
      'PUT /api/v1/manual/cutter_off': (req, res) => {
        currentState.cutterRotating = false;
      },
      'PUT /api/v1/manual/forward': (req, res) => {},
      'PUT /api/v1/manual/backward': (req, res) => {},
      'PUT /api/v1/manual/turn': (req, res) => {},
      'PUT /api/v1/manual/stop': (req, res) => {},
      'GET /api/v1/history/battery': {
        samples: batterySamples()
      },
      'GET /api/v1/system' : (req, res) => {
        return res.json({
          name: 'liam-esp',
          version: '1.0.0',
          uptime: Math.round((new Date().getTime() - uptime.getTime()) / 1000),
          cpuFreq: 240,
          flashChipSize: 4194304,
          freeHeap: 109608,
          wifiSignal: Math.floor(Math.random() * (-30 - -90 + 1)) + -90
        });
      },

      'PUT /api/v1/reboot': (req, res) => {}
    };

module.exports = proxy;
