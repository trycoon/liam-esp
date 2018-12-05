import * as api from '../rest.js';

let sec = $('.js-section-start');

function setLaunchMowerState() {
  api.selectState("LAUNCHING")
  .fail(function(e) {
    console.error(e.message);
  });
}

function setMowingState() {
  api.selectState("MOWING")
  .fail(function(e) {
    console.error(e.message);
  });
}

function setDockingState() {
  api.selectState("DOCKING")
  .fail(function(e) {
    console.error(e.message);
  });
}

function setStopState() {
  api.selectState("STOP")
  .fail(function(e) {
    console.error(e.message);
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

function updateBattery() {
  if (liam.data.status && liam.data.status.hasOwnProperty('batteryLevel')) {
    let batteryTicks = document.querySelectorAll('.js-battery .js-tick');
  
    if (liam.data.status.isCharging) {
      document.querySelector('.js-charging').style.visibility = 'visibility';
      document.querySelector('.js-battery-value').style.display = 'none';

      batteryTicks.forEach(function(tick) {
        tick.style.fill = "#880";
        tick.classList.add('pulsate');
      });

    } else {
      document.querySelector('.js-charging').style.visibility = 'hidden';
      let batteryValueEl = document.querySelector('.js-battery-value');
      batteryValueEl.textContent = liam.data.status.batteryLevel + '%';
      batteryValueEl.style.display = 'inline-block';

      batteryTicks.forEach(function(tick) {
        tick.classList.remove('pulsate');
      });
      
      batteryTicks[0].style.fill = liam.data.status.batteryLevel >= 100 ? "#080" : "#b3b3b3";
      batteryTicks[1].style.fill = liam.data.status.batteryLevel > 90 ? "#080" : "#b3b3b3";
      batteryTicks[2].style.fill = liam.data.status.batteryLevel > 80 ? "#080" : "#b3b3b3";
      batteryTicks[3].style.fill = liam.data.status.batteryLevel > 70 ? "#080" : "#b3b3b3";
      batteryTicks[4].style.fill = liam.data.status.batteryLevel > 60 ? "#080" : "#b3b3b3";
      batteryTicks[5].style.fill = liam.data.status.batteryLevel >= 50 ? "#080" : "#b3b3b3";
  
      batteryTicks[6].style.fill = liam.data.status.batteryLevel >= 50 ? "#080" : liam.data.status.batteryLevel > 40 ? "#880" : "#b3b3b3";
      batteryTicks[7].style.fill = liam.data.status.batteryLevel >= 50 ? "#080" : liam.data.status.batteryLevel > 30 ? "#880" : "#b3b3b3";
  
      batteryTicks[8].style.fill = liam.data.status.batteryLevel >= 50 ? "#080" : liam.data.status.batteryLevel > 20 ? "#880" : liam.data.status.batteryLevel > 10 ? "#800" : "#b3b3b3";
      batteryTicks[9].style.fill = liam.data.status.batteryLevel >= 50 ? "#080" : liam.data.status.batteryLevel > 20 ? "#880" : liam.data.status.batteryLevel > 0 ? "#800" : "#b3b3b3";
    }
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
  updateBattery();
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

  //https://github.com/kchapelier/PRWM
  //https://www.youtube.com/watch?v=kB0ZVUrI4Aw
  let mower3dCanvas = document.querySelector('.js-gl3dmodel');
  let gl = mower3dCanvas.getContext('webgl');

  if (!gl) {
    gl = mower3dCanvas.getContext('experimental-webgl');
  }

  if (!gl) {
    console.error('Browser does not support WebGL, 3D preview of mower will not be available.');
    return;
  }
  
  gl.clearColor(0.1, 0.13, 0.15, 1.0);
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

}
