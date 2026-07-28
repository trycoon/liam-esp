import * as api from '../api.js';
let interval,
    requestInProgress,
    logmessages = [],
    infoSection = document.querySelector('.js-section-info'),
    syslogTextArea = document.getElementById('syslog'),
    syslogTailCheckbox = document.getElementById('tailSyslog'),
    lastLogNr = 0;

function renderInfo() {
    let info = liam.data.system;
    infoSection.querySelector('.appName').textContent = info.name;
    infoSection.querySelector('.appVersion').textContent = info.version;
    infoSection.querySelector('.mowerId').textContent = info.mowerId;
    infoSection.querySelector('.localTime').textContent = info.localTime;
}

function getSystemInfoAndRender() {
    if (requestInProgress) {
        return;
    }

    requestInProgress = true;

    Promise.all([
        api.getSystem(),
        api.getLogmessages(lastLogNr),
    ]).then(([system, logs]) => {
        liam.data.system = system;
        renderInfo();

        logmessages = logmessages.concat(logs.messages);
        lastLogNr = logs.lastnr;

        syslogTextArea.value =  logmessages.join('\n');

        if (syslogTailCheckbox.checked) {
            syslogTextArea.scrollTop = syslogTextArea.scrollHeight;
        }
    })
    .catch((error) => {
        console.error(error.message);
    })
    .finally(() => {
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

    infoSection.querySelector('.wifiSignal').textContent = `${wifiQuality} (${status.wifiSignal} dBm)`;
    infoSection.querySelector('.uptime').innerHTML = uptimeFormat(status.uptime);
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
