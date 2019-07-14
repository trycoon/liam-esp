import * as api from '../api.js';
let interval,
    requestInProgress,
    logmessages = [],
    lastLogNr = 0;

function renderInfo() {
    let info = liam.data.system;
    $('.js-section-info .appName').text(info.name);
    $('.js-section-info .appVersion').text(info.version);
    $('.js-section-info .mowerId').text(info.mowerId);
    $('.js-section-info .localTime').text(info.localTime);
}

function getSystemInfoAndRender() {
    if (requestInProgress) {
        return;
    }

    requestInProgress = true;

    $.when(
        api.getSystem(),
        api.getLogmessages(lastLogNr)
    ).done((r1, r2) => {
        liam.data.system = r1[0];
        renderInfo();

        logmessages = logmessages.concat(r2[0].messages);
        lastLogNr = r2[0].lastnr;

        $('#syslog').text(logmessages.join('\n'));
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
    renderInfo();   // render the information we already got
    getSystemInfoAndRender();   // and then get fresh ones
    interval = setInterval(() => {
        getSystemInfoAndRender();   // and keep on fetching updated system information as long as the user view this tab
    }, 5000);
}

export function unselected() {
    clearInterval(interval);
}

export function init() {
    window.addEventListener('statusUpdated', updatedStatus);
}
