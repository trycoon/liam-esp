liam.sections = liam.sections || {};
liam.sections.manual =
(function() {
  'use strict';

  let sec = $('.js-section-manual');

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

  function init() {

    sec.find('.js-startmower').on('click', function() {
      startMowerMotor();
    });
    sec.find('.js-stopmower').on('click', function() {
      stopMowerMotor();
    });

    $('.joyknob')[0].getBBox();
  }

  function selected() {
  }

  function unselected() {
   }

  return {    
    init: init,
    selected: selected,
    unselected: unselected
  }
})();
