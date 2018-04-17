/**
 * Liam-ESP emulator
 * Simple Node.js application that tries to emulate a Liam-ESP lawnmover by serving the files the mower normally does and answering the same REST API-calls.
 * This is usefull when you want to develop the GUI without having a lawnmover at hand (or risking starting the cutter and chopping of your hands).
 * 
 * Start emulator:
 * 1. install Node.js (https://nodejs.org)
 * 2. "npm install" (installs all dependencies)
 * 3. "node index" (start emulator)
 * 4. point webbrowser to "http://localhost:3000"
 */
const express = require('express');
      bodyParser = require('body-parser'),
      app = express();
let currentState = { state: 'DOCKED',
                     cutterRotating: false };

app.use(express.static('data'));
app.use(bodyParser.json())
app.set('json spaces', 40);
app.get('/api/v1/status', apiGetStatus);
app.put('/api/v1/state', apiSetState);
app.put('/api/v1/manual/cutter_on', apiSetCutterOn);
app.put('/api/v1/manual/cutter_off', apiSetCutterOff);

app.listen(3000, () => console.log('Liam-ESP emulator listening on port 3000.'));

function apiGetStatus(req, res) {
    res.json(currentState);
}

function apiSetState(req, res) {
    currentState.state = req.body;
    res.json(currentState);
}

function apiSetCutterOn(req, res) {
    currentState.cutterRotating = true;
    res.json(currentState);
}

function apiSetCutterOff(req, res) {
    currentState.cutterRotating = false;
    res.json(currentState);
}