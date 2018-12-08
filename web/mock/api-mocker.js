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
      data = require(resolve(__dirname, 'mock-data.js')),
      Cookies = require('cookies');

function isAuthenticated(req, res) {
  let cookies = new Cookies(req, res);
  let sessionVal = cookies.get(`liam-${data.getCurrentSystem().mowerId}`);
  return sessionVal === '1234567';
} 

let proxy = {
      'GET /api/v1/status': (req, res) => {
        if (isAuthenticated(req, res)) {
          return res.json(data.getCurrentState());
        } else {
          res.sendStatus(401);
        }
      },
      'PUT /api/v1/state': (req, res) => {
        if (isAuthenticated(req, res)) {
          data.setState(req.body.state);
          res.sendStatus(200);
        } else {
          res.sendStatus(401);
        }
      },
      'PUT /api/v1/manual/cutter_on': (req, res) => {
        if (isAuthenticated(req, res)) {
          data.cutterOn();
          res.sendStatus(200);
        } else {
          res.sendStatus(401);
        }
      },
      'PUT /api/v1/manual/cutter_off': (req, res) => {
        if (isAuthenticated(req, res)) {
          data.cutterOff();
          res.sendStatus(200);
        } else {
          res.sendStatus(401);
        }
      },
      'PUT /api/v1/manual/forward': (req, res) => {
        if (isAuthenticated(req, res)) {
          res.sendStatus(200);
        } else {
          res.sendStatus(401);
        }
      },
      'PUT /api/v1/manual/backward': (req, res) => {
        if (isAuthenticated(req, res)) {
          res.sendStatus(200);
        } else {
          res.sendStatus(401);
        }
      },
      'PUT /api/v1/manual/turn': (req, res) => {
        if (isAuthenticated(req, res)) {
          res.sendStatus(200);
        } else {
          res.sendStatus(401);
        }
      },
      'PUT /api/v1/manual/stop': (req, res) => {
        if (isAuthenticated(req, res)) {
          res.sendStatus(200);
        } else {
          res.sendStatus(401);
        }
      },
      'GET /api/v1/history/battery': (req, res) => {
        if (isAuthenticated(req, res)) {
          return res.json({samples: data.getBatterySamples()});
        } else {
          res.sendStatus(401);
        }        
      },
      'GET /api/v1/system' : (req, res) => {
        if (isAuthenticated(req, res)) {
          return res.json(data.getCurrentSystem());
        } else {
          res.sendStatus(401);
        }
      },
      'PUT /api/v1/reboot': (req, res) => {
        if (isAuthenticated(req, res)) {
          res.sendStatus(200);
        } else {
          res.sendStatus(401);
        }
      },
      'PUT /api/v1/factoryreset': (req, res) => {
        if (isAuthenticated(req, res)) {
          res.sendStatus(200);
        } else {
          res.sendStatus(401);
        }
      },
      'GET /api/v1/loglevel': (req, res) => {
        if (isAuthenticated(req, res)) {
          return res.json(data.getLoglevel());
        } else {
          res.sendStatus(401);
        }        
      },
      'PUT /api/v1/loglevel': (req, res) => {
        if (isAuthenticated(req, res)) {
          data.setLoglevel(req.body.level);
          res.sendStatus(200);
        } else {
          res.sendStatus(401);
        }
      },
      'GET /api/v1/logmessages': (req, res) => {
        if (isAuthenticated(req, res)) {
          return res.json(data.getLogmessages());
        } else {
          res.sendStatus(401);
        }
      },
      'POST /api/v1/session': (req, res) => {
        let cookies = new Cookies(req, res);
        console.dir(req.body);
        if (req.body.username === 'admin' && req.body.password === 'liam') {
          cookies.set(`liam-${data.getCurrentSystem().mowerId}`, '1234567', {
            path: '/api',
          });
          res.sendStatus(200);
        } else {
          res.sendStatus(401);
        }
      },
      'GET /api/v1/session': (req, res) => {
        if (isAuthenticated(req, res)) {
          res.sendStatus(200);
        } else {
          res.sendStatus(401);
        }
      },
      'DELETE /api/v1/session': (req, res) => {
        let cookies = new Cookies(req, res);
        cookies.set(`liam-${data.getCurrentSystem().mowerId}`, 'null', {
          path: '/api',
          maxAge: 0,
        });
        res.sendStatus(200);
      },

      'POST /api/v1/apikey': (req, res) => {
        if (isAuthenticated(req, res)) {
          const CHARS = '0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz';
          let key = 'xxxxxxxxxxxxxxxx'.replace(/[x]/g, function(c) {
            return CHARS[Math.floor(Math.random() * CHARS.length)];
          });
  
          data.setApiKey(key);
          res.sendStatus(200);
        } else {
          res.sendStatus(401);
        }
      },
      'POST /updatefirmware': (req, res) => {
        if (isAuthenticated(req, res)) {
          res.writeHead(303, {'Content-Type': 'text/plain', 'Location': '/'});
          res.end('SUCCESS');
        } else {
          res.sendStatus(401);
        }
      },
    };

module.exports = proxy;
