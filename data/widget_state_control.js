liam.widgets = liam.widgets || {};
liam.widgets.state_control =
(function() {

  function start() {
    console.log('Starting "state-control" widget');
  }

  function getStyles() {
    return $(`
      .state-control .state {
        font-size: 20px;
        line-height: 1.3;
        margin-bottom: 10px;
        white-space: nowrap;
        text-overflow: ellipsis;
        text-align: center
        animation: colorFade 3s infinite
        @keyframes colorFade {
            0% {fill: #477090}
            50% {fill: #073050}
            100% {fill: #477090}
        }
      }
      .state-control .buttons {
        margin-bottom: 10px;
      }
      .state-control .button {
        display: inline-block;
        padding: 5px 7px;
        cursor: pointer;
      }
    `);
  }

  function getDom() {

    this.$el = $(`
      <div class="box state-control">
        <div class="heading">Läge</div>
        <div class="state">Dockad</div>
        <div class="buttons">
          <span class="js-launching button">Påbörja klippning</span>
          <span class="js-mowing button">Fortsätt klippa</span>
          <span class="js-docking button">Ladda klippare</span>
          <span class="js-paused button">Pausa klippning</span>
          <span class="js-test button">Testprogram</span>
        </div>
      </div>
    `);

  	this.$el.find('.js-launching').on('click', function() {
      liam.rest.selectState("LAUNCHING")
      .done(function() {
        alert('LAUNCHING');
      }).error(function() {
        alert('error');
      });
  	});
    this.$el.find('.js-mowing').on('click', function() {
      liam.rest.selectState("MOWING")
      .done(function() {
        alert('MOWING');
      }).error(function() {
        alert('error');
      });
    });
    this.$el.find('.js-docking').on('click', function() {
      liam.rest.selectState("DOCKING")
      .done(function() {
        alert('DOCKING');
      }).error(function() {
        alert('error');
      });
    });
    this.$el.find('.js-paused').on('click', function() {
      liam.rest.selectState("PAUSED")
      .done(function() {
        alert('PAUSED');
      }).error(function() {
        alert('error');
      });
    });
    this.$el.find('.js-test').on('click', function() {
      liam.rest.selectState("TEST")
      .done(function() {
        alert('TEST');
      }).error(function() {
        alert('error');
      });
    });

    this.$el.css({
      'opacity': 0.4
    });

    return this.$el;
  }

  function updateDom() {

  }

  return {
    start: start,
    getStyles: getStyles,
    getDom: getDom,
    updateDom: updateDom
  }
})();
