import * as api from '../rest.js';

let joypad = {
  el: $('.joypad')[0],
  centerX: 0,
  centerY: 0,
  radius: 0,
},
 joyknob = {
  el: $('.joyknob')[0],
  centerX: 0,
  centerY: 0,
  radius: 0,
  isDragging: false,
  grabPointX: undefined,
  grabPointY: undefined,
},
 trueCoords = null,
 SVGRoot = null;

var knob;
export function selected() {
}

export function unselected() {
}

function startMowerMotor() {
  api.manual("cutter_on")
  .fail(function(e) {
    alert('Error: '+ e);
  });
}

function stopMowerMotor() {
  api.manual("cutter_off")
  .fail(function(e) {
    alert('Error: '+ e);
  });
}

function initJoystick(evt) {
  SVGRoot = evt.target;
  trueCoords = evt.target.createSVGPoint();

  /*let padbox = $('.joypad')[0].getBBox();
  joypad.centerX = padbox.width / 2 + padbox.x;
  joypad.centerY = padbox.height / 2 + padbox.y;
  joypad.radius = padbox.width / 2;
  let knobbox = $('.joyknob')[0].getBBox();
  joyknob.centerX = knobbox.width / 2 + knobbox.x;
  joyknob.centerY = knobbox.height / 2 + knobbox.y;
  joyknob.radius = knobbox.width / 2;*/
}

function startDragJoystick(evt) {
  // we can only interact with joystick knob.
  if (evt.target === joyknob.el) {
    joyknob.isDragging = true;
    let transMatrix = evt.target.getCTM();
    joyknob.grabPointX = trueCoords.x - Number(transMatrix.e);
    joyknob.grabPointY = trueCoords.y - Number(transMatrix.f);
  }
}

function dragJoystick(evt) {
  if (joyknob.isDragging) {
     // find the current zoom level and pan setting, and adjust the reported
     // mouse position accordingly
     let newScale = SVGRoot.currentScale,
         translation = SVGRoot.currentTranslate;
     trueCoords.x = (evt.clientX - translation.x) / newScale;
     trueCoords.y = (evt.clientY - translation.y) / newScale;

    // account for the offset between the element's origin and the
    // exact place we grabbed it... this way, the drag will look more natural
    let newX = trueCoords.x - joyknob.grabPointX,
        newY = trueCoords.y - joyknob.grabPointY;
    joyknob.el.setAttributeNS(null, 'transform', 'translate(' + newX + ',' + newY + ')')
  }
}

function endDragJoystick(evt) {
  if (evt.target === joyknob.el) {
    //evt.target.setAttributeNS(null, 'pointer-events', 'all');
    joyknob.isDragging = false;
  }
}

export function init() {
  
  let sec = $('.js-section-manual');
  sec.find('.js-startmower').on('click', function() {
    startMowerMotor();
  });
  sec.find('.js-stopmower').on('click', function() {
    stopMowerMotor();
  });

  $('.joystick').on('load', initJoystick);
  $('.joystick').on('mousemove', dragJoystick);
  $('.joystick').on('mousedown', startDragJoystick);
  $('.joystick').on('mouseup', endDragJoystick);  

  // https://github.com/slobdell/joystick
  // view-source:http://svg-whiz.com/svg/DragAndDrop.svg
  // http://www.petercollingridge.co.uk/tutorials/svg/interactive/dragging/
  // http://www.petercollingridge.co.uk/interactive-svg-components/draggable-svg-element
}


