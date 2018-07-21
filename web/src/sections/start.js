import * as api from '../rest.js';

let sec = $('.js-section-start'),
    compassSVG,
    compassArrow;

function setLaunchMowerState() {
  api.selectState("LAUNCHING")
  .fail(function() {
    console.error(e);
  });
}

function setMowingState() {
  api.selectState("MOWING")
  .fail(function() {
    console.error(e);
  });
}

function setDockingState() {
  api.selectState("DOCKING")
  .fail(function() {
    console.error(e);
  });
}

function setStopState() {
  api.selectState("STOP")
  .fail(function() {
    console.error(e);
  });
}

function toggleStateButtons() {

  if (liam.data.status.state) {
    switch (liam.data.status.state) {
      case 'DOCKED': {
        $('.js-launching').show();
        $('.js-docking').hide();
        $('.js-mowing').hide();
        $('.js-stop').hide();
        break;
      }
      case 'LAUNCHING': {
        $('.js-launching').hide();
        $('.js-docking').show();
        $('.js-mowing').hide();
        $('.js-stop').show();
        break;
      }
      case 'MOWING': {
        $('.js-launching').hide();
        $('.js-docking').show();
        $('.js-mowing').hide();
        $('.js-stop').show();
        break;
      }
      case 'DOCKING': {
        $('.js-launching').show();
        $('.js-docking').hide();
        $('.js-mowing').show();
        $('.js-stop').show();
        break;
      }
      case 'CHARGING': {
        $('.js-launching').show();
        $('.js-docking').hide();
        $('.js-mowing').hide();
        $('.js-stop').hide();
        break;
      }
      case 'STUCK': {
        $('.js-launching').hide();
        $('.js-docking').show();
        $('.js-mowing').show();
        $('.js-stop').hide();
        break;
      }
      case 'FLIPPED': {
        $('.js-launching').hide();
        $('.js-docking').show();
        $('.js-mowing').show();
        $('.js-stop').hide();
        break;
      }
      case 'STOP': {
        $('.js-launching').hide();
        $('.js-docking').show();
        $('.js-mowing').show();
        $('.js-stop').hide();
        break;
      }
      case 'MANUAL': {
        $('.js-launching').hide();
        $('.js-docking').show();
        $('.js-mowing').show();
        $('.js-stop').show();
        break;
      }
      case 'TEST': {
        $('.js-launching').hide();
        $('.js-docking').show();
        $('.js-mowing').show();
        $('.js-stop').show();
        break;
      }
      default: {
        console.log('Unknown state: ' + liam.data.status.state);
      }
    }
  }
}

function updateCompass() {
  if (compassSVG) {
    let translate = compassSVG.createSVGTransform();
    // find center of arrow and rotate around it.
    let bb = compassArrow.getBBox();
    let cx = bb.x + bb.width / 2;
    let cy = bb.y + bb.height / 2;
    translate.setRotate(liam.data.status.heading, cx, cy);
    compassArrow.transform.baseVal.clear(); // reset transformation between updates.
    compassArrow.transform.baseVal.appendItem(translate);
   }
}

function updatedStatus() {

  if (!liam.data.status) {
    return;
  }
  
  let text;
  switch (liam.data.status.state) {
    case 'DOCKED': text = 'DOCKED'; break;
    case 'LAUNCHING': text = 'LAUNCHING'; break;
    case 'MOWING': text = 'MOWINGR'; break;
    case 'DOCKING': text = 'DOCKING'; break;
    case 'CHARGING': text = 'CHARGING'; break;
    case 'STUCK': text = 'STUCK'; break;
    case 'FLIPPED': text = 'FLIPPED'; break;
    case 'MANUAL': text = 'MANUAL'; break;
    case 'STOP': text = 'STOP'; break;
    case 'TEST': text = 'TEST'; break;
    default: text = '...';
  }

  sec.find('.js-state').text(text);

  toggleStateButtons();
  updateCompass();
}

export function selected() {
}

export function unselected() {
}

export function init() {

  window.addEventListener('statusUpdated', updatedStatus);
  updatedStatus();

  sec.find('.js-launching').on('click', function() {
    setLaunchMowerState();
  });
  sec.find('.js-mowing').on('click', function() {
    setMowingState();
  });
  sec.find('.js-docking').on('click', function() {
    setDockingState();
  });
  sec.find('.js-stop').on('click', function() {
    setStopState();
  });

  $('.js-compass').on('load', (evt) => {
    compassSVG = evt.target;
    compassArrow = $('.js-compass .js-arrow')[0];
  });
}
