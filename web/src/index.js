import './styles/main.scss'
import * as api from './rest.js';
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
    status: {}
  },
  sections: {
    start: sectionStart,
    manual: sectionManual,
    metrics: sectionMetrics,
    settings: sectionSettings,
    schedule: sectionSchedule,
    info: sectionInfo
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
  document.getElementById('modal').style.display = 'block';
}

function hideLostConnectionModal() {
  document.getElementById('modal').style.display = 'none';
}

function startSubscribingOnStatus() {
  api.getStatus().done(data => {
    liam.data.status = data;
    window.dispatchEvent(new Event('statusUpdated'));
  });

  socket = new ReconnectingWebSocket('ws://' + location.host + '/ws');

  socket.addEventListener('open', () => {
    console.info('Got WS connection.');
    if (socketDisconnectedTimeout) {
      clearTimeout(socketDisconnectedTimeout);
      socketDisconnectedTimeout = undefined;
    }
    hideLostConnectionModal();
  });

  socket.addEventListener('close', () => {
    console.info('Lost WS connection.');
    // show lost connection modal if we have not been able to reconnect within 2 seconds.
    socketDisconnectedTimeout = setTimeout(() => {
      showLostConnectionModal();
    }, 2000);    
  });

  socket.addEventListener('error', (error) => {
    console.warn(`Got WS error: ${error}`);
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

function init() {
  // Hide all sections first, showSection() will show the appropriate one.
  $('.section').hide();
  $('.js-main-nav').on('click', '.js-main-nav-item', function() {
    showSection($(this).data('section'));
  });

  setTheme();
  addClickEffect();

  for (let section in global.liam.sections) {
    global.liam.sections[section].init();
  }
  
  showSection('start');
  showLostConnectionModal();
  startSubscribingOnStatus();
}

// Start application.
init();