liam.sections = liam.sections || {};
liam.sections.start =
(function() {
  let statusPoller,
      el = $('js-section-start');

  el.find('.js-launching').on('click', function() {
    liam.rest.selectState("LAUNCHING")
    .done(function() {
      alert('LAUNCHING');
    }).error(function() {
      alert('error');
    });
  });
  el.find('.js-mowing').on('click', function() {
    liam.rest.selectState("MOWING")
    .done(function() {
      alert('MOWING');
    }).error(function() {
      alert('error');
    });
  });
  el.find('.js-docking').on('click', function() {
    liam.rest.selectState("DOCKING")
    .done(function() {
      alert('DOCKING');
    }).error(function() {
      alert('error');
    });
  });
  el.find('.js-paused').on('click', function() {
    liam.rest.selectState("PAUSED")
    .done(function() {
      alert('PAUSED');
    }).error(function() {
      alert('error');
    });
  });
  el.find('.js-test').on('click', function() {
    liam.rest.selectState("TEST")
    .done(function() {
      alert('TEST');
    }).error(function() {
      alert('error');
    });
  });

  function selected() {
    statusPoller = setInterval(() => {
      liam.rest.getStatus().done(data => {
        updateStatus(data);
      });
    }, 2000);
  }

  function unselected() {
    clearInterval(statusPoller);
  }

  function updateStatus(data) {
    el.find('.js-state').text(data.state);

  }
/*

  function getDom() {


    this.$el.css({
      'opacity': 0.4
    });

    return this.$el;
  }
*/

  return {
    selected: selected,
    unselected: unselected
  }
})();
