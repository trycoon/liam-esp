import * as api from '../api.js';
import * as mapUtils from '../maputils.js';
import { Joystick } from '../components/joystick.js';

let joystick;

export function selected() {
  // TODO: https://github.com/taroz/GNSS-Radar ("skyplot"), switch charist to Highchart for more functionality.
}

export function unselected() {
   
}

export function init() {

  joystick = new Joystick(document.getElementById('map_joystick'));
  /*joystick.on('stop', () => stop());
  joystick.on('move', movement => {
    api.socketSend(movement.forward ? "forward" : "backward", {
      speed: movement.speed,
      turnrate: movement.turnrate,
      smooth: true
    });
  });*/

}
