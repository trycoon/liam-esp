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
      cutterRotating: false,
      batteryVoltage: 14,
      batteryLevel: 90,
      cutterLoad: 30,
      dir: 90,
      pitch: 0,
      roll: 0,
      yaw: 45
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
    proxy = {
      'GET /api/v1/status': currentState,
      'PUT /api/v1/state': (req, res) => {
        currentState.state = req.body.state;
      },
      'PUT /api/v1/manual/cutter_on': (req, res) => {
        currentState.cutterRotating = true;
      },
      'PUT /api/v1/manual/cutter_off': (req, res) => {
        currentState.cutterRotating = false;
      },
      'GET /api/v1/history/battery': {
        samples: batterySamples()
      },
      'GET /api/v1/system' : {
        name: 'liam-esp',
        version: '1.0.0',
        cpuFreq: 80,
        flashChipSize: 1234,
        freeHeap: 40000,
        wifiSignal: -37
      },

      'PUT /api/v1/reboot': (req, res) => {}
    };

module.exports = proxy;
