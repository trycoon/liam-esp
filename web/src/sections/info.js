import * as api from '../rest.js';
let interval;

function getInfoAndRender() {
    api.getSystem().done(data => {
        $('.js-section-info .appName').text(data.name);
        $('.js-section-info .appVersion').text(data.version);
        $('.js-section-info .uptime').text(data.uptime); //TODO: print text: "xx hour yy min zz sec"
        $('.js-section-info .cpuFreq').text(data.cpuFreq);
        $('.js-section-info .flashChipSize').text(data.flashChipSize);
        $('.js-section-info .freeHeap').text(data.freeHeap);
        $('.js-section-info .wifiSignal').text(data.wifiSignal);
    });
}

export function selected() {
    getInfoAndRender();

    interval = setInterval(() => {
        getInfoAndRender();
    }, 5000);
}

export function unselected() {
    clearInterval(interval);
}

export function init() {}
