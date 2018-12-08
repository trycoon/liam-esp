import * as api from '../rest.js';
const MAX_SAMPLES = 40; // How many points in graph, too many gets hard to read.
let batteryChart,
    batteryData = [],
    batteryFullThreshold = new Array(MAX_SAMPLES),
    batteryEmptyThreshold = new Array(MAX_SAMPLES),
    wifiChart,
    wifiData = [],
    cutterLoadChart,
    cutterLoadData = [],
    batteryInterval,
    batteryRequestInProgress;

function getBatteryHistoryAndRender() {
    if (batteryRequestInProgress) {
        return;
    }

    batteryRequestInProgress = true;

    api.getBatteryHistory()
    .then(function(data) {
        let values = data.samples.value,
            times = data.samples.time;
        // limit number of samples
        if (values.length > MAX_SAMPLES) {
            values = values.slice(-MAX_SAMPLES);
            times = times.slice(-MAX_SAMPLES);
        }

        batteryChart.update({
            series: [{
                name: 'batteryVoltage',
                data: values        
            },
            {
                name: 'batteryFullThreshold',
                data: batteryFullThreshold        
            },
            {
                name: 'batteryEmptyThreshold',
                data: batteryEmptyThreshold        
            }],
        });
    })
    .catch(function(e) {
        console.error(e.message);
    })
    .always(() => {
        batteryRequestInProgress = false;
    });
}

// Keep receiving and storing useful metrics, that we could display later
function updatedStatus() {
    // prevent chart from growing to infinity (consuming browser memory)
    if (cutterLoadData.length > MAX_SAMPLES) {
        cutterLoadData = cutterLoadData.slice(-MAX_SAMPLES);
    }
    if (wifiData.length > MAX_SAMPLES) {
        wifiData = wifiData.slice(-MAX_SAMPLES);
    }

    cutterLoadData.push(liam.data.status.cutterLoad);
    wifiData.push(liam.data.status.wifiSignal);

    cutterLoadChart.update({
        series: [cutterLoadData],
    });

    wifiChart.update({
        series: [wifiData],
    });
}

export function selected() {
    getBatteryHistoryAndRender();
    batteryInterval = setInterval(() => {
        getBatteryHistoryAndRender();   // and keep on fetching updated battery status as long as the user view this tab
    }, 5000);
}

export function unselected() {
    clearInterval(batteryInterval);
}

export function init() {

    window.addEventListener('statusUpdated', updatedStatus);

    batteryFullThreshold.fill(liam.data.system.settings.batteryFullVoltage);
    batteryEmptyThreshold.fill(liam.data.system.settings.batteryEmptyVoltage);

    batteryChart = new Chartist.Line('#battery-chart', {
        series: [{
            name: 'batteryVoltage',
            data: batteryData        
        },
        {
            name: 'batteryFullThreshold',
            data: batteryFullThreshold        
        },
        {
            name: 'batteryEmptyThreshold',
            data: batteryEmptyThreshold        
        }],
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
        high: liam.data.system.settings.batteryFullVoltage + 1.0,
        low: liam.data.system.settings.batteryEmptyVoltage - 1.0,
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