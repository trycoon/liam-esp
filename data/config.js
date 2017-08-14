window.liam.config = window.liam.config ||
{
  theme: 'default',
  sections: [
    {
      section: "start",
      title: "Start",
      margins: [10, 10],
      base_dimensions: [180, 140],
      icon: 'home'
    },
    {
      section: "metrics",
      title: "Statistik",
      margins: [10, 10],
      base_dimensions: [180, 140],
      icon: 'bar-chart'
    },
    {
      section: "settings",
      title: "Inställningar",
      margins: [10, 10],
      base_dimensions: [180, 140],
      icon: 'cogs'
    },
    {
      section: "info",
      title: "Information",
      margins: [10, 10],
      base_dimensions: [180, 140],
      icon: 'info-circle'
    }
  ],
  sectionsWidgets: {
    "start": [
      {
        widget: "mower_illustration",
        config: {
          column: 1,
          row: 1,
          size_x: 2,
          size_y: 2
        }
      },
      {
        widget: "state_control",
        config: {
          title: "Läge",
          column: 1,
          row: 3,
          size_x: 10,
          size_y: 20
        }
      },
      {
        widget: "battery_level",
        config: {
          title: "Batterinivå",
          column: 1,
          row: 5,
          size_x: 2,
          size_y: 1
        }
      }
    ],
    "metrics": [
      {
        widget: "graph",
        config: {
          type: "battery",
          title: "Batterinivå",
          column: 1,
          row: 1,
          size_x: 4,
          size_y: 2
        }
      },
      {
        widget: "graph",
        config: {
          type: "cutter",
          title: "Klippar belastning",
          column: 1,
          row: 3,
          size_x: 4,
          size_y: 2
        }
      },
      {
        widget: "graph",
        config: {
          type: "memory",
          title: "Minnesanvändning",
          column: 1,
          row: 5,
          size_x: 4,
          size_y: 2
        }
      }
    ],
    "settings": [

    ],
    "info": [
      {
        widget: "system_info",
        config: {
          title: "Systeminformation",
          column: 1,
          row: 1,
          size_x: 3,
          size_y: 4
        }
      }
    ]
  }
};
