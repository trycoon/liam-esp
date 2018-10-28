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

const { resolve } = require('path'),
      data = require(resolve(__dirname, 'mock-data.js'));

let proxy = {
      'GET /api/v1/status': (req, res) => {
        return res.json(data.getCurrentState());
      },
      'PUT /api/v1/state': (req, res) => {
        data.setState(req.body.state);
        res.sendStatus(200);
      },
      'PUT /api/v1/manual/cutter_on': (req, res) => {
        data.cutterOn();
        res.sendStatus(200);
      },
      'PUT /api/v1/manual/cutter_off': (req, res) => {
        data.cutterOff();
        res.sendStatus(200);
      },
      'PUT /api/v1/manual/forward': (req, res) => {
        res.sendStatus(200);
      },
      'PUT /api/v1/manual/backward': (req, res) => {
        res.sendStatus(200);
      },
      'PUT /api/v1/manual/turn': (req, res) => {
        res.sendStatus(200);
      },
      'PUT /api/v1/manual/stop': (req, res) => {
        res.sendStatus(200);
      },
      'GET /api/v1/history/battery': {
        samples: data.getBatterySamples()
      },
      'GET /api/v1/system' : (req, res) => {
        return res.json(data.getCurrentSystem());
      },
      'PUT /api/v1/reboot': (req, res) => {
        res.sendStatus(200);
      },
      'PUT /api/v1/factoryreset': (req, res) => {
        res.sendStatus(200);
      },
      'GET /api/v1/loglevel': (req, res) => {
        return res.json(data.getLoglevel());
      },
      'PUT /api/v1/loglevel': (req, res) => {
        data.setLoglevel(req.body.level);
        res.sendStatus(200);
      },
      'GET /api/v1/logmessages': (req, res) => {
        return res.json(data.getLogmessages());
      },
      'POST /api/v1/apikey': (req, res) => {
        const CHARS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        let key = 'xxxxxxxxxxxxxxxx'.replace(/[x]/g, function(c) {
          return CHARS[Math.floor(Math.random() * CHARS.length)];
        });

        data.setApiKey(key);
        res.sendStatus(200);
      },
    };

module.exports = proxy;
