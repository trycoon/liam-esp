import * as api from '../rest.js';

let sec = $('.js-section-start');

function setLaunchMowerState() {
  api.selectState("LAUNCHING")
  .fail(function() {
    alert('error');
  });
}

function setMowingState() {
  api.selectState("MOWING")
  .fail(function() {
    alert('error');
  });
}

function setDockingState() {
  api.selectState("DOCKING")
  .fail(function() {
    alert('error');
  });
}

function setPausedState() {
  api.selectState("PAUSED")
  .fail(function() {
    alert('error');
  });
}

function toggleStateButtons() {
  $('.js-launching').show();
  $('.js-docking').show();
  $('.js-paused').show();
  $('.js-mowing').show();

  if (liam.data.status.state) {
    switch (liam.data.status.state) {
      case 'DOCKED': {
        $('.js-launching').show();
        $('.js-docking').hide();
        $('.js-paused').hide();
        $('.js-mowing').hide();
        break;
      }
      case 'LAUNCHING': {
        $('.js-launching').hide();
        $('.js-docking').show();
        $('.js-paused').show();
        $('.js-mowing').show();
        break;
      }
      case 'MOWING': {
        $('.js-launching').hide();
        $('.js-docking').show();
        $('.js-paused').show();
        $('.js-mowing').hide();
        break;
      }
      case 'DOCKING': {
        $('.js-launching').show();
        $('.js-docking').hide();
        $('.js-paused').show();
        $('.js-mowing').show();
        break;
      }
      case 'CHARGING': {
        $('.js-launching').show();
        $('.js-docking').hide();
        $('.js-paused').hide();
        $('.js-mowing').hide();
        break;
      }
      case 'STUCK': {
        $('.js-launching').hide();
        $('.js-docking').show();
        $('.js-paused').hide();
        $('.js-mowing').show();
        break;
      }
      case 'FLIPPED': {
        $('.js-launching').hide();
        $('.js-docking').show();
        $('.js-paused').hide();
        $('.js-mowing').show();
        break;
      }
      case 'PAUSED': {
        $('.js-launching').hide();
        $('.js-docking').show();
        $('.js-paused').hide();
        $('.js-mowing').show();
        break;
      }
      case 'TEST': {
        $('.js-launching').hide();
        $('.js-docking').show();
        $('.js-paused').show();
        $('.js-mowing').show();
        break;
      }
      default: {
        console.log('Unknown state: ' + liam.data.status.state);
      }
    }
  }
}

function updatedStatus() {

  if (!liam.data.status) {
    return;
  }
  
  let text;
  switch (liam.data.status.state) {
    case 'DOCKED': text = 'PARKERAD'; break;
    case 'LAUNCHING': text = 'PÅBÖRJAR KLIPPNING'; break;
    case 'MOWING': text = 'KLIPPNING PÅGÅR'; break;
    case 'DOCKING': text = 'PARKERAR'; break;
    case 'CHARGING': text = 'LADDAR'; break;
    case 'STUCK': text = 'FASTNAT'; break;
    case 'FLIPPED': text = 'UPPOCHNED VÄND'; break;
    case 'MANUAL': text = 'MANUELL KÖRNING'; break;
    case 'PAUSED': text = 'PAUSAD'; break;
    case 'TEST': text = 'TESTKÖRNING'; break;
    default: text = '...';
  }

  sec.find('.js-state').text(text);

  toggleStateButtons();
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
  sec.find('.js-paused').on('click', function() {
    setPausedState();
  });
}
