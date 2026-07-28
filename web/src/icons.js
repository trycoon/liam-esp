// Material Design Icons using @mdi/js - completely tree-shakeable.
import {
  mdiClose,
  mdiAccount,
  mdiHome,
  mdiGamepad,
  mdiChartBar,
  mdiClock,
  mdiRoad,
  mdiCog,
  mdiAlertCircle,
  mdiPowerPlug,
  mdiDelete
} from '@mdi/js';

const ICON_PATHS = {
  ban: mdiClose,
  account: mdiAccount,
  home: mdiHome,
  gamepad: mdiGamepad,
  chart: mdiChartBar,
  clock: mdiClock,
  route: mdiRoad,
  settings: mdiCog,
  info: mdiAlertCircle,
  plug: mdiPowerPlug,
  trash: mdiDelete
};

function createMdiSvg(pathData, size = 24, className = '', title = '') {
  if (!pathData || typeof document === 'undefined') return null;

  const svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
  svg.setAttribute('viewBox', '0 0 24 24');
  svg.setAttribute('width', String(size));
  svg.setAttribute('height', String(size));
  svg.setAttribute('fill', 'currentColor');
  svg.setAttribute('aria-hidden', title ? 'false' : 'true');
  if (title) {
    svg.setAttribute('role', 'img');
    svg.setAttribute('aria-label', title);
  }
  if (className) {
    svg.setAttribute('class', className);
  }

  svg.style.display = 'inline-block';
  svg.style.verticalAlign = 'middle';

  const path = document.createElementNS('http://www.w3.org/2000/svg', 'path');
  path.setAttribute('d', pathData);
  svg.appendChild(path);

  return svg;
}

export function mountMdiIcon(target, iconName, options = {}) {
  if (typeof document === 'undefined') return;

  const element = typeof target === 'string' ? document.querySelector(target) : target;
  if (!element) return;

  const pathData = ICON_PATHS[iconName];
  if (!pathData) return;

  const size = options.size ?? 24;
  const className = options.className ?? '';
  const title = options.title ?? '';

  const svg = createMdiSvg(pathData, size, className, title);
  if (!svg) return;

  element.replaceChildren(svg);
}

export function getMdiIconMarkup(iconName, options = {}) {
  const pathData = ICON_PATHS[iconName];
  if (!pathData) return '';

  const size = options.size ?? 24;
  const className = options.className ? ` class="${options.className}"` : '';
  const title = options.title ? ` aria-label="${options.title}" role="img"` : ' aria-hidden="true"';

  return `<svg viewBox="0 0 24 24" width="${size}" height="${size}" fill="currentColor"${className}${title}><path d="${pathData}"></path></svg>`;
}

export function renderStaticAppIcons() {
  const iconTargets = [
    ['.js-icon-no-connection', 'ban', 48],
    ['.js-icon-login', 'account', 36],
    ['.js-icon-nav-start', 'home', 24],
    ['.js-icon-nav-manual', 'gamepad', 24],
    ['.js-icon-nav-metrics', 'chart', 24],
    ['.js-icon-nav-schedule', 'clock', 24],
    ['.js-icon-nav-map', 'route', 24],
    ['.js-icon-nav-settings', 'settings', 24],
    ['.js-icon-nav-info', 'info', 24],
    ['.js-icon-charging', 'plug', 24]
  ];

  iconTargets.forEach(([selector, iconName, size]) => {
    mountMdiIcon(selector, iconName, {
      size,
      className: 'app-icon'
    });
  });
}

export const mdiIcons = {
  trash: 'trash'
};
