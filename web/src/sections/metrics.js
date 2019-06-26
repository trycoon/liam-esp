import * as api from '../api.js';
const MAX_SAMPLES = 40; // How many points in graph, too many gets hard to read.
let batteryChart,
    batteryData = [],
    batteryFullThreshold = new Array(MAX_SAMPLES),
    batteryEmptyThreshold = new Array(MAX_SAMPLES),
    wifiChart,
    wifiData = [],
    cutterLoadChart,
    cutterLoadData = [],
    memoryChart,
    memoryHeapFree = [],
    memoryMaxAllocHeap = [],
    pollInterval,
    requestInProgress;

// get metrics from REST-API by polling
function getMetricsAndRender() {
    if (requestInProgress) {
        return;
    }

    requestInProgress = true;

    $.when(
        api.getBatteryHistory(),
        api.getSystem()
    ).done((battery, system) => {
        let values = battery[0].samples.value,
            times = battery[0].samples.time;
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

        liam.data.system = system[0];
        // prevent chart from growing to infinity (consuming browser memory)
        if (memoryHeapFree.length > MAX_SAMPLES) {
            memoryHeapFree = memoryHeapFree.slice(-MAX_SAMPLES);
        }
        memoryHeapFree.push(Math.round(liam.data.system.freeHeap / 1024)); 

        if (memoryMaxAllocHeap.length > MAX_SAMPLES) {
            memoryMaxAllocHeap = memoryMaxAllocHeap.slice(-MAX_SAMPLES);
        }
        memoryMaxAllocHeap.push(Math.round(liam.data.system.getMaxAllocHeap / 1024));

        memoryChart.update({
            series: [{
                name: 'memoryHeapFree',
                data: memoryHeapFree        
            },
            {
                name: 'memoryMaxAllocHeap',
                data: memoryMaxAllocHeap        
            }],
        });
    })
    .catch(function(e) {
        console.error(e.message);
    })
    .always(() => {
        requestInProgress = false;
    });
}

// Keep receiving and storing useful metrics from Websocket
function updatedStatus() {
    // prevent chart from growing to infinity (consuming browser memory)
    if (cutterLoadData.length > MAX_SAMPLES) {
        cutterLoadData = cutterLoadData.slice(-MAX_SAMPLES);
    }
    cutterLoadData.push(liam.data.status.cutterLoad);
    cutterLoadChart.update({
        series: [cutterLoadData],
    });

    // prevent chart from growing to infinity (consuming browser memory)
    if (wifiData.length > MAX_SAMPLES) {
        wifiData = wifiData.slice(-MAX_SAMPLES);
    }
    wifiData.push(liam.data.status.wifiSignal);
    wifiChart.update({
        series: [wifiData],
    });
}

export function selected() {
    getMetricsAndRender();
    pollInterval = setInterval(() => {
        getMetricsAndRender();   // and keep on fetching updated metrics as long as the user view this tab
    }, 3000);
}

export function unselected() {
    clearInterval(pollInterval);
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

    memoryChart = new Chartist.Line('#memory-chart', {
        series: [{
                name: 'memoryHeapFree',
                data: memoryHeapFree        
            },
            {
                name: 'memoryMaxAllocHeap',
                data: memoryMaxAllocHeap        
            }],
    }, {
        /*plugins: [
            Chartist.plugins.legend({
                legendNames: ['free', 'min free', 'max block'],
                position: 'bottom'
            })
        ],*/
        axisX: {
            showGrid: false,
        },
        axisY: {
            showLabel: true,
        },
        showPoint: false,
        high: Math.round(liam.data.system.totalHeap / 1024),
        low: 0,
    });
}