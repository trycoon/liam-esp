window.liam = (function() {
  'use strict';

  let loadedSections = {},
      currentActiveSection;

  function init() {
    fixClickEvents();
    setTheme();
    initNavAndSections();
    showSection('start');
    startPollingStatus();
  }

  function fixClickEvents() {
    // Add click effect on widgets that are clickable
    $(document).on('click', '.clickable', function(e) {
      let target = $(this);

      target.addClass('clickable-active');

      setTimeout(function() {
        target.removeClass('clickable-active');
      }, 20);
    });
  }

  function setTheme(name) {
    $('html').addClass(name ? 'theme-' + name : liam.config.theme ? 'theme-' + liam.config.theme : 'theme-default');
  }

  function initNavAndSections() {
    let navList = $('.js-nav-left-list'),
        main = $('.js-main'),
        menuWidth = $('.js-nav-left').outerWidth(),
        contentWidth = $(window).width() - menuWidth,
        isMobile = $(window).width() < 480,
        sectionName,
        baseDimensions,
        margins,
        maxCols;

    liam.config.sections.forEach(function(section) {
      sectionName = section.section;

      navList.append($('<li class="nav-left-item js-nav-left-item js-nav-section-' + sectionName + '" data-section="' + sectionName + '"><span class="fa fa-' + section.icon + ' fa-3x"></span></li>'));

      let sectionEl = $('<section class="section js-section js-section-' + sectionName +'" />');
      main.append(sectionEl);

      baseDimensions = section.base_dimensions;
      margins = section.margins;

      if (isMobile) {
        margins = [5, 5];

        let baseWidth = baseDimensions[0],
            marginHorizontal = margins[0];

        // One or two columns. Could be done better..
        if (baseWidth > contentWidth) {
          baseDimensions = [contentWidth - marginHorizontal * 2, baseDimensions[1]];
          maxCols = 1;
        } else {
          baseDimensions = [(contentWidth - marginHorizontal * 3) / 2, baseDimensions[1]];
          maxCols = 2;
        }
      }
    });
    // Hide all sections first, showSection() will show the appropriate one.
    $('.section').hide();

    $('.js-nav-left').on('click', '.js-nav-left-item', function() {
      showSection($(this).data('section'));
    });
  }

  function showSection(section) {
    let sections = $('.js-section'),
        navItems = $('.js-nav-left-item'),
        sectionEl = $('.js-section-' + section),
        navSectionEl = $('.js-nav-section-' + section);

    navItems.removeClass('active');
    navSectionEl.addClass('active');
    sections.hide();
    sectionEl.show();

    if (currentActiveSection) {
      currentActiveSection.unselected();
    }

    // dynamic load section if it has not already been loaded.
    if (!loadedSections.hasOwnProperty(section)) {
      let confSection = liam.config.sections.find(
        sec => sec.section === section
      );

      sectionEl.load(confSection.html, () => {
        loadedSections[section] = true;
        currentActiveSection = liam.sections[section];
        currentActiveSection.init();
        currentActiveSection.selected();
      });
    } else {
      currentActiveSection = liam.sections[section];
      currentActiveSection.selected();
    }
  }

  function startPollingStatus() {
    liam.rest.getStatus().done(data => {
      liam.data.status = data;
      window.dispatchEvent(new Event('statusUpdated'));
    });

    setInterval(() => {
      liam.rest.getStatus().done(data => {
        if (JSON.stringify(data) !== JSON.stringify(liam.data.status)) {
          liam.data.status = data;
          window.dispatchEvent(new Event('statusUpdated'));
        }
      });
    }, 2000);
  }

  return {
    init: init,
    setTheme: setTheme
  }
})();

// store for local state.
window.liam.data = {
  status: {}
};

window.liam.config = {
  theme: 'default',
  sections: [
    {
      section: 'start',
      title: 'Start',
      margins: [10, 10],
      base_dimensions: [180, 140],
      icon: 'home',
      html: 'sections/start.html'
    },
    {
      section: 'manual',
      title: 'Manuell',
      margins: [10, 10],
      base_dimensions: [180, 140],
      icon: 'gamepad',
      html: 'sections/manual.html'
    },
    {
      section: 'metrics',
      title: 'Mätvärden',
      margins: [10, 10],
      base_dimensions: [180, 140],
      icon: 'bar-chart',
      html: 'sections/metrics.html'
    },
    {
      section: 'settings',
      title: 'Inställningar',
      margins: [10, 10],
      base_dimensions: [180, 140],
      icon: 'cogs',
      html: 'sections/settings.html'
    },
    {
      section: 'info',
      title: "Information",
      margins: [10, 10],
      base_dimensions: [180, 140],
      icon: 'info-circle',
      html: 'sections/info.html'
    }
  ]
};
