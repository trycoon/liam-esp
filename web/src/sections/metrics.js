import * as api from '../rest.js';
const MAX_SAMPLES = 20;
let batteryChart,
    batteryData = [],
    wifiChart,
    wifiData = [],
    cutterLoadChart,
    cutterLoadData = [];

function renderUpdates() {
    wifiChart.update({
        series: [wifiData],
    });

    batteryChart.update({
        series: [batteryData],
    });

    cutterLoadChart.update({
        series: [cutterLoadData],
    });
}

// Keep receiving and storing useful metrics, that we could display later
function updatedStatus() {
    // prevent chart from growing to infinity (consuming browser memory)
    if (batteryData.length > MAX_SAMPLES) {
        batteryData = batteryData.slice(-MAX_SAMPLES);
    }
    if (cutterLoadData.length > MAX_SAMPLES) {
        cutterLoadData = cutterLoadData.slice(-MAX_SAMPLES);
    }
    if (wifiData.length > MAX_SAMPLES) {
        wifiData = wifiData.slice(-MAX_SAMPLES);
    }
    batteryData.push(liam.data.status.batteryVoltage);
    cutterLoadData.push(liam.data.status.cutterLoad);
    wifiData.push(liam.data.status.wifiSignal);

    renderUpdates();
}

export function selected() {
    renderUpdates();
}

export function unselected() {
}

export function init() {
    window.addEventListener('statusUpdated', updatedStatus);

    batteryChart = new Chartist.Line('#battery-chart', {
        series: [batteryData],
    }, {
        axisX: {
            showGrid: false,
        },
        axisY: {
            showLabel: true,
            labelInterpolationFnc: function(value) {
                return value.toFixed(1);
            },
        },
        //fullWidth: true,
        showPoint: false,
        high: 17.0,
        low: 13.0,
    });

    wifiChart = new Chartist.Line('#wifi-chart', {
        series: [wifiData],
    }, {
        axisX: {
            showGrid: false,
        },
        axisY: {
            showLabel: true,
        },
        showPoint: false,
        high: -20,
        low: -100,
    });

    cutterLoadChart = new Chartist.Line('#cutterload-chart', {
        series: [cutterLoadData],
    }, {
        axisX: {
            showGrid: false,
        },
        axisY: {
            showLabel: true,
            labelInterpolationFnc: function(value) {
                return Math.round(value);
            },
        },
        showPoint: false,
        high: 100,
        low: 0,
    });
}