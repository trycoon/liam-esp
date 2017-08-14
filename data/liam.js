window.liam = window.liam || (function() {

  var widgets = [];
  var sectionGridsters = {};

  function init() {
    FastClick.attach(document.body);

    // Add click effect on widgets that are clickable
    $(document).on('click', '.box-clickable', function(e) {
      var target = $(this);

      target.addClass('box-clickable-active');

      setTimeout(function() {
        target.removeClass('box-clickable-active');
      }, 20);
    });

    setTheme();
    renderSections();
    initNavigation();
    initWidgets();
  }

  function setTheme(name) {
    $('html').addClass(name ? 'theme-' + name : liam.config.theme ? 'theme-' + liam.config.theme : 'theme-default');
  }

  function renderSections() {
    var navList = $('.js-nav-left-list'),
        main = $('.js-main'),
        menuWidth = $('.js-nav-left').outerWidth(),
        contentWidth = $(window).width() - menuWidth,
        isMobile = $(window).width() < 480;

    liam.config.sections.forEach(function(section) {

      var navSectionEl = $('<li class="nav-left-item js-nav-left-item" data-section="' + section.section + '"><span class="fa fa-' + section.icon + ' fa-3x"></span></li>');

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
        show_element: function($el, callback) {
          $el.show();
          if (callback) {
            callback();
          } 
        },
        hide_element: function($el, callback) {
          $el.hide();
          if (callback) {
            callback();
          } 
        },
        shift_widgets_up: false,
        shift_larger_widgets_down: false,
      }).data('gridster').disable(); // Disable drag-and-drop
    });
  }

  function initNavigation() {
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

  function initWidgets() {
    Object.keys(liam.config.sectionsWidgets).forEach(function(section) {
      liam.config.sectionsWidgets[section].forEach(function(sectionWidgetConfig) {
        if (liam.widgets.hasOwnProperty(sectionWidgetConfig.widget)) {
          let widget = liam.widgets[sectionWidgetConfig.widget];

          widget.config = sectionWidgetConfig;
          widget.config.section = section;

          widget.start();

          widgets.push(widget);
        }
      });
    });

    renderWidgets();
  }

  function renderWidgets() {
    widgets.forEach(function(widget) {
      var widgetDOM = widget.getDom();

      sectionGridsters[widget.config.section].add_widget(widgetDOM, widget.config.size_x || 1, widget.config.size_y || 1, widget.config.column || 1, widget.config.row || 1);
    });
  }

  return {
    init: init,
    setTheme: setTheme
  }
})();
