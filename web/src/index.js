import { renderStaticAppIcons } from './icons.js';
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

function setTheme(name) {
  document.documentElement.classList.add(name ? 'theme-' + name : liam.config.theme ? 'theme-' + liam.config.theme : 'theme-default');
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
  const sections = document.querySelectorAll('.js-section');
  const navItems = document.querySelectorAll('.js-main-nav-item');
  const sectionEl = document.querySelector('.js-section-' + section);
  const navSectionEl = document.querySelector('.js-nav-section-' + section);

  navItems.forEach((item) => {
    item.classList.remove('active');
  });

  if (navSectionEl) {
    navSectionEl.classList.add('active');
  }

  sections.forEach((sectionNode) => {
    sectionNode.style.display = 'none';
  });

  if (sectionEl) {
    sectionEl.style.display = 'block';
  }

  if (currentActiveSection) {
    currentActiveSection.unselected();
  }

  currentActiveSection = liam.sections[section];
  currentActiveSection.selected();
}

function initialSetup() {
  // get initial settings and system information.
  Promise.all([
    api.getSystem(),
    api.getStatus(),
  ]).then(([system, status]) => {
    liam.data.system = system;
    liam.data.status = status;

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
  renderStaticAppIcons();

  // Hide all sections first, showSection() will show the appropriate one.
  document.querySelectorAll('.section').forEach((sectionNode) => {
    sectionNode.style.display = 'none';
  });

  document.querySelector('.js-main-nav').addEventListener('click', (event) => {
    const navItem = event.target.closest('.js-main-nav-item');
    if (!navItem) {
      return;
    }

    showSection(navItem.dataset.section);
  });

  setTheme();
  initialSetup();
}

// Start application.
init();