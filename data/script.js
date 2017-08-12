FastClick.attach(document.body);

// Add click effect on modules that are clickable
$(document).on('click', '.box-clickable', function(e) {
  var target = $(this);

  target.addClass('box-clickable-active');

  setTimeout(function() {
    target.removeClass('box-clickable-active');
  }, 20);
});

var liam = (function() {
  var sectionGridsters = {};

  var renderSections = function() {
    var navList = $('.js-nav-left-list'),
        main = $('.js-main'),
        menuWidth = $('.js-nav-left').outerWidth(),
        contentWidth = $(window).width() - menuWidth,
        isMobile = $(window).width() < 480;

    for (var i = 0, sectionLength = config.sections.length; i < sectionLength; i++) {
      var section = config.sections[i];

      var navSectionEl = $('<li class="nav-left-item js-nav-left-item" data-section="' + section.section + '"><i class="material-icons md-36">' + section.icon + '</i>' + section.title + '</li>');

      navList.append(navSectionEl);

      var sectionEl = $('<section class="section section-' + section.section +'" />');

      main.append(sectionEl);

      var baseDimensions = section.base_dimensions,
          margins = section.margins,
          maxCols;

      if (isMobile) {
        margins = [5, 5];

        var baseWidth = baseDimensions[0],
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

      sectionGridsters[section.section] = sectionEl.gridster({
        namespace: '.section-' + section.section,
        widget_selector: '.box',
        max_cols: maxCols,
        widget_margins: margins,
        widget_base_dimensions: baseDimensions,
        show_element: function($el, callback) { $el.show(); if (callback) { callback(); } },
        hide_element: function($el, callback) { $el.hide(); if (callback) { callback(); } },
        shift_widgets_up: false,
        shift_larger_widgets_down: false,
      }).data('gridster').disable(); // Disable drag-and-drop
    }
  }

  var initNavigation = function() {
    var sections = $('.section'),
        navItems = $('.js-nav-left-item');

    sections.hide().first().show();
    navItems.first().addClass('active');

    $('.js-nav-left').on('click', '.js-nav-left-item', function() {
      var section = $(this).data('section'),
          sectionEl = $('.section-' + section);

      // TODO: Add some fancy animation
      sections.hide();
      sectionEl.show();

      navItems.removeClass('active');
      $(this).addClass('active');
    });
  }

  var setTheme = function() {
    $('html').addClass(config.theme ? 'theme-' + config.theme : 'theme-default');
  }

  return {
    renderSections: renderSections,
    initNavigation: initNavigation,
    setTheme: setTheme
  }
})();
