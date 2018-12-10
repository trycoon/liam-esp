import './styles/main.scss'
import * as api from './rest.js';
import * as auth from './authorisation.js';
import * as sectionInfo from './sections/info.js';
import * as sectionManual from './sections/manual.js';
import * as sectionMetrics from './sections/metrics.js';
import * as sectionSettings from './sections/settings.js';
import * as sectionSchedule from './sections/schedule.js';
import * as sectionStart from './sections/start.js';
import ReconnectingWebSocket from 'reconnecting-websocket';

// store for local state.
global.liam = {
  config: {},
  data: {
    status: {},
    system: {},
  },
  sections: {
    start: sectionStart,
    manual: sectionManual,
    metrics: sectionMetrics,
    settings: sectionSettings,
    schedule: sectionSchedule,
    info: sectionInfo,
  }
};

let currentActiveSection,
    isSmallDisplay = $(window).width() < 480,
    socket,
    socketDisconnectedTimeout;

function addClickEffect() {
  // Add click effect on widgets that are clickable
  $(document).on('click', '.clickable', function(e) {
    let target = $(this);

    target.addClass('clickable-active');

    setTimeout(() => {
      target.removeClass('clickable-active');
    }, 20);
  });
}

function setTheme(name) {
  $('html').addClass(name ? 'theme-' + name : liam.config.theme ? 'theme-' + liam.config.theme : 'theme-default');
}

function showSection(section) {
  let sections = $('.js-section'),
      navItems = $('.js-main-nav-item'),
      sectionEl = $('.js-section-' + section),
      navSectionEl = $('.js-nav-section-' + section);

  navItems.removeClass('active');
  navSectionEl.addClass('active');
  sections.hide();
  sectionEl.show();

  if (currentActiveSection) {
    currentActiveSection.unselected();
  }

  currentActiveSection = liam.sections[section];
  currentActiveSection.selected();
}

function showLostConnectionModal() {
  document.querySelector('.js-no-connection-modal').style.display = 'block';
  document.querySelector('.js-loginbox').style.display = 'none';
  document.getElementById('modal').style.display = 'block';
}

function hideLostConnectionModal() {
  document.getElementById('modal').style.display = 'none';
  document.querySelector('.js-loginbox').style.display = 'none';
}

function startSubscribingOnStatus() {
  let protocol = location.protocol.indexOf('https') === 0 ? 'wss' : 'ws';
  socket = new ReconnectingWebSocket(`${protocol}://${location.host}/ws`);

  socket.addEventListener('open', () => {
    console.info('Got WS connection.');
    if (socketDisconnectedTimeout) {
      clearTimeout(socketDisconnectedTimeout);
      socketDisconnectedTimeout = undefined;
    }
    if (!auth.isLoginDialogVisible()) {
      hideLostConnectionModal();
    }
  });

  socket.addEventListener('close', () => {
    console.info('Lost WS connection.');
    // show lost connection modal if we have not been able to reconnect within 2 seconds.
    socketDisconnectedTimeout = setTimeout(() => {
      // if we are not currently logging in, show lost connection warning.
      if (!auth.isLoginDialogVisible()) {
        showLostConnectionModal();
      }
    }, 2000);    
  });

  socket.addEventListener('error', (error) => {
    console.warn(`Got WS error: ${error.message}`);
  });

  // Listen for messages
  socket.addEventListener('message', function (event) {
    let message = JSON.parse(event.data);

    if (message.type === 'status') {
      let status = message.payload;
      if (JSON.stringify(status) !== JSON.stringify(liam.data.status)) {
        liam.data.status = status;
        window.dispatchEvent(new Event('statusUpdated'));
      }
    }
  });  
}

function initialSetup() {
  // get initial settings and system information.
  $.when(api.getSystem(), api.getStatus())
  .done((system, status) => {
    liam.data.system = system[0];
    liam.data.status = status[0];
    showLostConnectionModal();

    for (let section in global.liam.sections) {
      global.liam.sections[section].init();
    }
  
    window.dispatchEvent(new Event('statusUpdated'));
    showSection('start');
    startSubscribingOnStatus();
  }).catch(error => {
    if (error.status === 401) {
      auth.showLogin().then(() => {
        initialSetup();
      });
    } else {
      setTimeout(initialSetup, 500); // retry if failed.
    }
  });
}

function init() {
  // Hide all sections first, showSection() will show the appropriate one.
  $('.section').hide();
  $('.js-main-nav').on('click', '.js-main-nav-item', function() {
    showSection($(this).data('section'));
  });

  setTheme();
  addClickEffect();
  initialSetup();
}

// Start application.
init();