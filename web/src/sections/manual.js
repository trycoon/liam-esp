import * as api from '../api.js';
import * as auth from '../authorisation.js';
import { Joystick } from '../components/joystick.js';

let startMowerButton,
    stopMowerButton,
    joystick;

export function selected() {
  stop();
}

export function unselected() {
  stop();
}

function stop() {
  api.manual("stop")
  .catch(error => {
    if (error.status === 401) {
      auth.showLogin().then(() => {
        stop();
      });
    } else {
      // keep on trying if we failed
      setTimeout(() => {
        stop();
      }, 400);
    }
  });
}

function startMowerMotor() {
  api.manual("cutter_on")
  .catch(error => {
    if (error.status === 401) {
      auth.showLogin().then(() => {
        startMowerMotor();
      });
    } else {
      console.error(error.message);
    }
  });
}

function stopMowerMotor() {
  api.manual("cutter_off")
  .catch(error => {
    if (error.status === 401) {
      auth.showLogin().then(() => {
        stopMowerMotor();
      });
    } else {
      // keep on trying if we failed
      setTimeout(() => {
        stopMowerMotor();
      }, 400);
    }
  });
}

function updatedStatus() {

  if (!liam.data.status) {
    return;
  }

  if (liam.data.status.cutterRotating) {
    startMowerButton.hide();
    stopMowerButton.show();
  } else {
    startMowerButton.show();
    stopMowerButton.hide();
  }
}

export function init() {
  
  let sec = $('.js-section-manual');

  startMowerButton = sec.find('.js-startmower');
  startMowerButton.on('click', () => {
    startMowerMotor();
  });

  stopMowerButton = sec.find('.js-stopmower');
  stopMowerButton.on('click', () => {
    stopMowerMotor();
  });

  joystick = new Joystick(document.getElementById('manual_joystick'));
  joystick.on('stop', () => stop());
  joystick.on('move', movement => {
    api.socketSend(movement.forward ? "forward" : "backward", {
      speed: movement.speed,
      turnrate: movement.turnrate,
      smooth: true
    }); 
  });

  window.addEventListener('statusUpdated', updatedStatus);
  updatedStatus();
}
