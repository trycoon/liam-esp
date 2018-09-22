import * as api from '../rest.js';

const DEADZONE_SIZE = 10; // Joystick center deadzone, to prevent drifting when knob is almost centered in the middle.

let joypad = {
  el: $('.joypad')[0],
  radius: parseFloat($('.joypad')[0].getAttributeNS(null, "r")),
 },
 joyknob = {
  el: $('.joyknob')[0],
  radius: parseFloat($('.joyknob')[0].getAttributeNS(null, "r")),
  isDragging: false,
  grabPoint: {},
  transform: undefined,
  turnrate: 0,
  speed: 0
 },
 maxLimit = joypad.radius - joyknob.radius,
 joystickSVG,
 sendCommandTimer,
 startMowerButton,
 stopMowerButton;

export function selected() {
  stop();
}

export function unselected() {
  stop();
}

function stop() {
  api.manual("stop")
  .fail(() => {
    // keep on trying if we failed
    setTimeout(() => {
      stop();
    }, 10);
  });
}

function startMowerMotor() {
  api.manual("cutter_on")
  .fail(function(e) {
    console.error(e);
  });
}

function stopMowerMotor() {
  api.manual("cutter_off")
  .fail(() => {
    // keep on trying if we failed
    setTimeout(() => {
      stopMowerMotor();
    }, 10);
  });
}

function initJoystick(evt) {

  joystickSVG = evt.target;

  joystickSVG.addEventListener('mousedown', startDrag);
  joystickSVG.addEventListener('touchstart', startDrag);
  joystickSVG.addEventListener('mousemove', drag);
  joystickSVG.addEventListener('touchmove', drag);
  joystickSVG.addEventListener('mouseup', endDrag);
  joystickSVG.addEventListener('mouseleave', endDrag);
  joystickSVG.addEventListener('touchend', endDrag);
  joystickSVG.addEventListener('touchleave', endDrag);
  joystickSVG.addEventListener('touchcancel', endDrag);

  function getMousePosition(evt) {
    let CTM = joystickSVG.getScreenCTM();
    // fix touch devices
    if (evt.touches) { 
      evt = evt.touches[0]; 
    }

    return {
      x: (evt.clientX - CTM.e) / CTM.a,
      y: (evt.clientY - CTM.f) / CTM.d
    };
  }

  function startDrag(evt) {
    // we can only interact with joystick knob.
    if (evt.target === joyknob.el) {
      joyknob.grabPoint = getMousePosition(evt);
      // Get all the transforms currently on this element
      let transforms = joyknob.el.transform.baseVal;
      // Ensure the first transform is a translate transform
      if (transforms.numberOfItems === 0 || transforms.getItem(0).type !== SVGTransform.SVG_TRANSFORM_TRANSLATE) {
        // Create an transform that translates by (0, 0)
        let translate = joystickSVG.createSVGTransform();
        translate.setTranslate(0, 0);
        // Add the translation to the front of the transforms list
        joyknob.el.transform.baseVal.insertItemBefore(translate, 0);
      }

      // Get initial translation amount
      joyknob.transform = transforms.getItem(0);
      joyknob.grabPoint.x -= joyknob.transform.matrix.e;
      joyknob.grabPoint.y -= joyknob.transform.matrix.f;
      joyknob.isDragging = true;
    }
  }
  
  function drag(evt) {
    if (joyknob.isDragging) {
      evt.preventDefault();

      let coord = getMousePosition(evt);
      coord.x = coord.x - joyknob.grabPoint.x;
      coord.y = coord.y - joyknob.grabPoint.y;

      if (valuesExceedRadius(coord.x, coord.y)) {
        // just add something to avoid divide-by-zero.
        if (coord.x === 0) {
          coord.x += 0.01;
        }

        let slope = coord.y / coord.x,
            xIncr = 0.01;

        if (coord.x < 0){
            xIncr = -0.01;
        }

        let x = 0,
            y = 0;

        // find intersection on joypad circle
        while (x < joypad.radius){
          y = x * slope;

          if (valuesExceedRadius(x, y)){
              break;
          }

          x += xIncr;
        }

        // some sanitychecks at top and bottom of joypad circle
        if (y < 0 && y < -maxLimit) {
          y = -maxLimit;
        } else if (y > 0 && y > maxLimit) {
          y = maxLimit;
        }

        coord.x = x;
        coord.y = y;
      }

      // visually move knob on joypad.
      joyknob.transform.setTranslate(coord.x, coord.y);

      // API related stuff bellow.
      joyknob.turnrate = coord.x;
      joyknob.speed = coord.y;

      if ((joyknob.turnrate < 0 && joyknob.turnrate > -DEADZONE_SIZE) || (joyknob.turnrate > 0 && joyknob.turnrate < DEADZONE_SIZE)) {
        joyknob.turnrate = 0;
      }
      
      if ((joyknob.speed < 0 && joyknob.speed > -DEADZONE_SIZE) || (joyknob.speed > 0 && joyknob.speed < DEADZONE_SIZE)) {
        joyknob.speed = 0;
      }

      // rescale values to the ranges expected by the API.
      let forward = joyknob.speed <= 0;
      joyknob.turnrate = Math.round((joyknob.turnrate + maxLimit) / (maxLimit * 2) * 200 - 100);
      joyknob.speed = Math.round(Math.abs(joyknob.speed) / maxLimit * 100);

      // don't SPAM mower with each and every milimeter of joystick movement. Let movement settle before sending command. 
      if (sendCommandTimer) {
        clearTimeout(sendCommandTimer);
      }

      sendCommandTimer = setTimeout(() => {
        api.manual(forward ? "forward" : "backward", {
          speed: joyknob.speed,
          turnrate: joyknob.turnrate,
          smooth: true
        });

      }, 100);
    }
  }
  
  function endDrag(evt) {
    if (evt.target === joyknob.el) {
      joyknob.isDragging = false;

      // ignore any pending movements, since we are stopping anyway.
      if (sendCommandTimer) {
        clearTimeout(sendCommandTimer);
      }

      stop();

      centerJoystickAtInactivity();
    }
  }

  function valuesExceedRadius(x, y){
    if(x === 0) {
        return y > joypad.radius - joyknob.radius;
    }

    return Math.sqrt(Math.pow(x, 2) + Math.pow(y, 2)) > joypad.radius - joyknob.radius;
  }

  function centerJoystickAtInactivity() {
    if (!joyknob.isDragging) {
      let speed = 0.7,
          xSign = 1,
          ySign = 1,
          x = joyknob.transform.matrix.e,
          y = joyknob.transform.matrix.f;

      // unless we have reached center, keep on translating towards center.
      if (x !== 0 || y !== 0) {
        if (x !== 0) {
          xSign = x / Math.abs(x);
        }
        if (y !== 0) {
            ySign = y / Math.abs(y);
        }
        // instead of using the "speed" constant we could translate along a slope,
        // that would look more natural as we would end up in the center at the same time for both x and y-axis.
        joyknob.transform.setTranslate(Math.floor(speed * Math.abs(x)) * xSign, Math.floor(speed * Math.abs(y)) * ySign);
        window.requestAnimationFrame(centerJoystickAtInactivity);
      }
    }
  }
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

  $('.joystick').on('load', initJoystick);

  window.addEventListener('statusUpdated', updatedStatus);
  updatedStatus();
}
