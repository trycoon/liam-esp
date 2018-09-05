import * as api from '../rest.js';
let interval, requestInProgress;

function getInfoAndRender() {
    if (requestInProgress) {
        return;
    }

    requestInProgress = true;

    api.getSystem().done(data => {
        $('.js-section-info .appName').text(data.name);
        $('.js-section-info .appVersion').text(data.version);
        $('.js-section-info .uptime').text(uptimeFormat(data.uptime));
        $('.js-section-info .cpuFreq').text(data.cpuFreq);
        $('.js-section-info .flashChipSize').text(data.flashChipSize);
        $('.js-section-info .freeHeap').text(data.freeHeap);
        $('.js-section-info .wifiSignal').text(data.wifiSignal);
    })
    .always(() => {
        requestInProgress = false;
    });
}

function uptimeFormat(seconds) {
    let day, hour, minute;

    minute = Math.floor(seconds / 60);
    seconds = seconds % 60;
    hour = Math.floor(minute / 60);
    minute = minute % 60;
    day = Math.floor(hour / 24);
    hour = hour % 24;
    
    return `${day} days, ${hour} hours, ${minute} minutes, ${seconds} seconds`;
}

export function selected() {
    interval = setInterval(() => {
        getInfoAndRender();
    }, 5000);
}

export function unselected() {
    clearInterval(interval);
}

export function init() {
    getInfoAndRender();
}
