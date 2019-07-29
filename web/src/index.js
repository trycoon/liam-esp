import './styles/main.scss'
import * as api from './api.js';
import * as auth from './authorisation.js';
import * as sectionMap from './sections/map.js';
import * as sectionInfo from './sections/info.js';
import * as sectionManual from './sections/manual.js';
import * as sectionMetrics from './sections/metrics.js';
import * as sectionSettings from './sections/settings.js';
import * as sectionSchedule from './sections/schedule.js';
import * as sectionStart from './sections/start.js';

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
    map: sectionMap,
  }
};

let currentActiveSection,
    lastUptime = 0;

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

window.addEventListener('statusUpdated', () => {
  // if mower has been restarted then reload client also. This is to make sure that we have the latest client-software running in case mower has been restarted due to a firmware update.
  if (liam.data.status.uptime < lastUptime) {
    location.reload(true);
  } else {
    lastUptime = liam.data.status.uptime;
  }
});

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

function initialSetup() {
  // get initial settings and system information.
  $.when(
    api.getSystem(),
    api.getStatus()
  ).done((system, status) => {
    liam.data.system = system[0];
    liam.data.status = status[0];

    for (let section in global.liam.sections) {
      global.liam.sections[section].init();
    }
  
    window.dispatchEvent(new Event('statusUpdated'));
    showSection('start');

    api.setupSocket();
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