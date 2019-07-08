const DEADZONE_SIZE = 5; // Joystick center deadzone, to prevent drifting when knob is almost centered in the middle.
const EventEmitter = require('events');

/**
 * Joystick component, initialize with an reference to the SVG DOM-element to control.
 * Subscribe on "stop" and "move"-events.
 */
export class Joystick extends EventEmitter {

  _getMousePosition(evt) {
    let CTM = this.joystickEl.getScreenCTM();
    // fix touch devices
    if (evt.touches) {
      evt = evt.touches[0];
    }

    return {
      x: (evt.clientX - CTM.e) / CTM.a,
      y: (evt.clientY - CTM.f) / CTM.d,
    };
  }

  _startDrag(evt) {
    // we can only interact with joystick knob.
    if (evt.target === this.joyknob.el) {
      this.joyknob.grabPoint = this._getMousePosition(evt);
      // Get all the transforms currently on this element
      let transforms = this.joyknob.el.transform.baseVal;
      // Ensure the first transform is a translate transform
      if (transforms.numberOfItems === 0 || transforms.getItem(0).type !== SVGTransform.SVG_TRANSFORM_TRANSLATE) {
        // Create an transform that translates by (0, 0)
        let translate = this.joystickEl.createSVGTransform();
        translate.setTranslate(0, 0);
        // Add the translation to the front of the transforms list
        this.joyknob.el.transform.baseVal.insertItemBefore(translate, 0);
      }

      // Get initial translation amount
      this.joyknob.transform = transforms.getItem(0);
      this.joyknob.grabPoint.x -= this.joyknob.transform.matrix.e;
      this.joyknob.grabPoint.y -= this.joyknob.transform.matrix.f;
      this.joyknob.isDragging = true;
    }
  }

  _centerJoystickAtInactivity() {
     if (!this.joyknob.isDragging) {
       let speed = 0.7,
         xSign = 1,
         ySign = 1,
         x = this.joyknob.transform.matrix.e,
         y = this.joyknob.transform.matrix.f;

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
         this.joyknob.transform.setTranslate(Math.floor(speed * Math.abs(x)) * xSign, Math.floor(speed * Math.abs(y)) * ySign);
         window.requestAnimationFrame(() => this._centerJoystickAtInactivity());
       }
     }
   }

  _endDrag(evt) {
     if (evt.target === this.joyknob.el) {
      this.joyknob.isDragging = false;

       // ignore any pending movements, since we are stopping anyway.
       if (this.sendCommandTimer) {
         clearTimeout(this.sendCommandTimer);
       }

       this.emit('stop');

       this._centerJoystickAtInactivity();
     }
   }

   _valuesExceedRadius(x, y) {
     if (x === 0) {
       return y > this.joypad.radius - this.joyknob.radius;
     }

     return Math.sqrt(Math.pow(x, 2) + Math.pow(y, 2)) > this.joypad.radius - this.joyknob.radius;
   }

   _drag(evt) {
     if (this.joyknob.isDragging) {
       evt.preventDefault();

       let coord = this._getMousePosition(evt);
       coord.x = coord.x - this.joyknob.grabPoint.x;
       coord.y = coord.y - this.joyknob.grabPoint.y;

       if (this._valuesExceedRadius(coord.x, coord.y)) {
         // just add something to avoid divide-by-zero.
         if (coord.x === 0) {
           coord.x += 0.01;
         }

         let slope = coord.y / coord.x,
           xIncr = 0.01;

         if (coord.x < 0) {
           xIncr = -0.01;
         }

         let x = 0,
           y = 0;

         // find intersection on joypad circle
         while (x < this.joypad.radius) {
           y = x * slope;

           if (this._valuesExceedRadius(x, y)) {
             break;
           }

           x += xIncr;
         }

         // some sanitychecks at top and bottom of joypad circle
         if (y < 0 && y < -this.maxLimit) {
           y = -this.maxLimit;
         } else if (y > 0 && y > this.maxLimit) {
           y = this.maxLimit;
         }

         coord.x = x;
         coord.y = y;
       }

       // visually move knob on joypad.
       this.joyknob.transform.setTranslate(coord.x, coord.y);

       // API related stuff bellow.
       this.joyknob.turnrate = coord.x;
       this.joyknob.speed = coord.y;

       if ((this.joyknob.turnrate < 0 && this.joyknob.turnrate > -DEADZONE_SIZE) || (this.joyknob.turnrate > 0 && this.joyknob.turnrate < DEADZONE_SIZE)) {
        this.joyknob.turnrate = 0;
       }

       if ((this.joyknob.speed < 0 && this.joyknob.speed > -DEADZONE_SIZE) || (this.joyknob.speed > 0 && this.joyknob.speed < DEADZONE_SIZE)) {
        this.joyknob.speed = 0;
       }

       // rescale values to the ranges expected by the API.
       let forward = this.joyknob.speed <= 0;
       this.joyknob.turnrate = Math.round((this.joyknob.turnrate + this.maxLimit) / (this.maxLimit * 2) * 200 - 100);
       this.joyknob.speed = Math.round(Math.abs(this.joyknob.speed) / this.maxLimit * 100);

       // don't SPAM mower with each and every milimeter of joystick movement. Let movement settle before sending command. 
       if (this.sendCommandTimer) {
         clearTimeout(this.sendCommandTimer);
       }

       this.sendCommandTimer = setTimeout(() => {

         let current = {
          forward: forward,
          speed: this.joyknob.speed,
          turnrate: this.joyknob.turnrate
         };

         // don't send same values over and over again.
         if (current.forward !== this.lastEmit.forward || current.speed !== this.lastEmit.speed || current.turnrate !== this.lastEmit.turnrate) {
          this.emit('move', current);
          this.lastEmit = current;
         }
       }, 50);
    }
   }

  _initJoystick() {

    this.joystickEl.addEventListener('mousedown', evt => this._startDrag(evt));
    this.joystickEl.addEventListener('touchstart', evt => this._startDrag(evt));
    this.joystickEl.addEventListener('mousemove', evt => this._drag(evt));
    this.joystickEl.addEventListener('touchmove', evt => this._drag(evt));
    this.joystickEl.addEventListener('mouseup', evt => this._endDrag(evt));
    this.joystickEl.addEventListener('mouseleave', evt => this._endDrag(evt));
    this.joystickEl.addEventListener('touchend', evt => this._endDrag(evt));
    this.joystickEl.addEventListener('touchleave', evt => this._endDrag(evt));
    this.joystickEl.addEventListener('touchcancel', evt => this._endDrag(evt));
  }

  constructor(joystickElement) {
    super();

    this.joystickEl = joystickElement;

    this.joypad = {
      el: joystickElement.querySelector('.joypad'),
      radius: parseFloat(joystickElement.querySelector('.joypad').getAttributeNS(null, "r")),
    };

    this.joyknob = {
      el: joystickElement.querySelector('.joyknob'),
      radius: parseFloat(joystickElement.querySelector('.joyknob').getAttributeNS(null, "r")),
      isDragging: false,
      grabPoint: {},
      transform: undefined,
      turnrate: 0,
      speed: 0,
    };

    this.maxLimit = this.joypad.radius - this.joyknob.radius;
    this.lastEmit = {};

    // detect if SVG yet has been loaded
    if (joystickElement.getCurrentTime() > 0) {
      this._initJoystick();
    } else {
      joystickElement.addEventListener('load', () => {
        this._initJoystick();
      });
    }
  }
}