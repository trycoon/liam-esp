import './styles/main.scss'
import * as api from './rest.js';
import * as sectionInfo from './sections/info.js';
import * as sectionManual from './sections/manual.js';
import * as sectionMetrics from './sections/metrics.js';
import * as sectionSettings from './sections/settings.js';
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
    info: sectionInfo
  }
};

let currentActiveSection,
    isSmallDisplay = $(window).width() < 480,
    socket;

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

function startSubscribingOnStatus() {
  api.getStatus().done(data => {
    liam.data.status = data;
    window.dispatchEvent(new Event('statusUpdated'));
  });

  socket = new ReconnectingWebSocket('ws://' + location.hostname + '/ws');

  // Listen for messages
  socket.addEventListener('message', function (event) {
    if (event.data !== JSON.stringify(liam.data.status)) {
      liam.data.status = JSON.parse(event.data);
      window.dispatchEvent(new Event('statusUpdated'));
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

  startSubscribingOnStatus();
}

// Start application.
init();