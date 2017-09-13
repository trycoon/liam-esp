window.liam = (function() {
  'use strict';
  
  let loadedSections = {},
      currentActiveSection;

  function init() {
    fixClickEvents();
    setTheme();
    initNavAndSections();
    showSection('start');
  }

  function fixClickEvents() {
    FastClick.attach(document.body);

    // Add click effect on widgets that are clickable
    $(document).on('click', '.box-clickable', function(e) {
      let target = $(this);

      target.addClass('box-clickable-active');

      setTimeout(function() {
        target.removeClass('box-clickable-active');
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
        currentActiveSection.selected();
      });
    } else {
      currentActiveSection = liam.sections[section];
      currentActiveSection.selected();
    }
  }

  return {
    init: init,
    setTheme: setTheme
  }
})();

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
