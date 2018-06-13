import * as api from '../rest.js';
const MAX_SAMPLES = 20;
let interval,
    batteryChart,
    batteryData = [],
    wifiChart,
    wifiData = [],
    cutterLoadChart,
    cutterLoadData = [];

function getInfoAndRender() {
    api.getSystem().done(data => {
        // prevent chart from growing to infinity (consuming browser memory)
        if (wifiData.length > MAX_SAMPLES) {
            wifiData = wifiData.slice(-MAX_SAMPLES);
        }
        wifiData.push(data.wifiSignal);
    
        wifiChart.update({
            series: [wifiData],
        });
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
    if (batteryData.length > MAX_SAMPLES) {
        batteryData = batteryData.slice(-MAX_SAMPLES);
    }
    if (cutterLoadData.length > MAX_SAMPLES) {
        cutterLoadData = cutterLoadData.slice(-MAX_SAMPLES);
    }
    batteryData.push(liam.data.status.batteryVoltage);
    cutterLoadData.push(liam.data.status.cutterLoad);
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

export function init() {
    window.addEventListener('statusUpdated', updatedStatus);

    batteryChart = new Chartist.Line('#battery-chart', {
        series: [batteryData],
    }, {
        width: 300,
        height: 200,
    });

    wifiChart = new Chartist.Line('#wifi-chart', {
        series: [wifiData],
    }, {
        width: 300,
        height: 200,
        /*fullWidth: true,
        axisX: {
            type: Chartist.AutoScaleAxis,
            showLabel: false,
            showGrid: true,
        },
        axisY: {
            offset: 0,
            showLabel: false,
            showGrid: false,
        },
        lineSmooth: Chartist.Interpolation.simple(),
        showLine: true,
        showPoint: true,
        showArea: true,*/
    });

    cutterLoadChart = new Chartist.Line('#cutterload-chart', {
        series: [cutterLoadData],
    }, {
        width: 300,
        height: 200,
    });
}