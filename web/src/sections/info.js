import * as api from '../rest.js';
let interval, requestInProgress;

function getSystemInfoAndRender() {
    if (requestInProgress) {
        return;
    }

    requestInProgress = true;

    $.when(
        api.getSystem(),
        api.getLogmessages()
    ).done((r1, r2) => {
        $('.js-section-info .appName').text(r1[0].name);
        $('.js-section-info .appVersion').text(r1[0].version);
        $('.js-section-info .cpuFreq').text(r1[0].cpuFreq);
        $('.js-section-info .flashChipSize').text(r1[0].flashChipSize);
        $('.js-section-info .freeHeap').text(r1[0].freeHeap);

        $('#syslog').text(r2[0].messages);
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
    
    return `<strong>${day}</strong> days, <strong>${hour}</strong> hours, <strong>${minute}</strong> min, <strong>${seconds}</strong> sec`;
}

function updatedStatus() {
    let status = liam.data.status;

    // https://www.metageek.com/training/resources/understanding-rssi.html

    let wifiQuality = '';
    if (status.wifiSignal > -40) {
        wifiQuality = 'Excellent';
    } else if (status.wifiSignal > -68) {
        wifiQuality = 'Very good';
    } else if (status.wifiSignal > -71) {
        wifiQuality = 'Good';
    } else if (status.wifiSignal > -81) {
        wifiQuality = 'Poor';
    } else {
        wifiQuality = 'Unusable';
    }

    $('.js-section-info .wifiSignal').text(`${wifiQuality} (${status.wifiSignal} dBm)`);
    $('.js-section-info .uptime').html(uptimeFormat(status.uptime));
}

export function selected() {
    interval = setInterval(() => {
        getSystemInfoAndRender();
    }, 5000);
}

export function unselected() {
    clearInterval(interval);
}

export function init() {
    window.addEventListener('statusUpdated', updatedStatus);
    getSystemInfoAndRender();
}
