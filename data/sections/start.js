liam.sections = liam.sections || {};
liam.sections.start =
(function() {
  'use strict';

  let statusPoller,
      currentStatus = {},
      sec = $('.js-section-start');

  function setLaunchMowerState() {
    liam.rest.selectState("LAUNCHING")
    .fail(function() {
      alert('error');
    });
  }

  function setMowingState() {
    liam.rest.selectState("MOWING")
    .fail(function() {
      alert('error');
    });
  }

  function setDockingState() {
    liam.rest.selectState("DOCKING")
    .fail(function() {
      alert('error');
    });
  }

  function setPausedState() {
    liam.rest.selectState("PAUSED")
    .fail(function() {
      alert('error');
    });
  }

  function setTestState() {
    liam.rest.selectState("TEST")
    .fail(function() {
      alert('error');
    });
  }

  function startMowerMotor() {
    liam.rest.manual("cutter_on")
    .fail(function() {
      alert('error');
    });
  }

  function stopMowerMotor() {
    liam.rest.manual("cutter_off")
    .fail(function() {
      alert('error');
    });
  }

  function selectAutomaticDrive() {
    $('.js-autodrive').show();
    $('.js-manualdrive').hide();
    $('.js-autodriveselector').hide();
    $('.js-manualdriveselector').show();

    toggleStateButtons();
  }

  function selectManualDrive() {
    $('.js-autodrive').hide();
    $('.js-manualdrive').show();
    $('.js-autodriveselector').show();
    $('.js-manualdriveselector').hide();
  }

  function toggleStateButtons() {
    $('.js-launching').show();
    $('.js-docking').show();
    $('.js-paused').show();
    $('.js-mowing').show();

    if (currentStatus.state) {
      switch (currentStatus.state) {
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
      }
    }
  }

  function setup() {
    selectAutomaticDrive();

    sec.find('.js-autodriveselector').on('click', function() {
      selectAutomaticDrive();
    });
    sec.find('.js-manualdriveselector').on('click', function() {
      selectManualDrive();
    });
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
    sec.find('.js-test').on('click', function() {
      setTestState();
    });
    sec.find('.js-startmower').on('click', function() {
      startMowerMotor();
    });
    sec.find('.js-stopmower').on('click', function() {
      stopMowerMotor();
    });

  }

  function selected() {
    liam.rest.getStatus().done(data => {
      updateStatus(data);
      toggleStateButtons();
    });

    statusPoller = setInterval(() => {
      liam.rest.getStatus().done(data => {
        updateStatus(data);
        toggleStateButtons();
      });
    }, 2000);
  }

  function unselected() {
    clearInterval(statusPoller);
  }

  function updateStatus(data) {
    currentStatus = data;

    let text;
    switch (data.state) {
      case 'DOCKED': text = 'Parkerad'; break;
      case 'LAUNCHING': text = 'Påbörjar klippning'; break;
      case 'MOWING': text = 'Klipper'; break;
      case 'DOCKING': text = 'Avslutar klippning'; break;
      case 'CHARGING': text = 'Laddar'; break;
      case 'STUCK': text = 'Fastnat'; break;
      case 'FLIPPED': text = 'Uppochned vänd'; break;
      case 'MANUAL': text = 'Manuell körning'; break;
      case 'PAUSED': text = 'Pausad'; break;
      case 'TEST': text = 'Testkörning'; break;
      default: text = '';
    }

    sec.find('.js-state').text(text);
  }

  setup();

  return {
    selected: selected,
    unselected: unselected
  }
})();
