import * as api from '../rest.js';
const MAX_SAMPLES = 20;
let interval,
    batteryChart,
    batteryData = [],
    wifiChart,
    cutterLoadChart,
    cutterLoadData = [];

function getInfoAndRender() {
    api.getSystem().done(data => {

        wifiChart.data.labels.push(''); // we dont't display labels but we need this to advance x-axis.
        wifiChart.data.datasets.forEach((dataset) => {
            // prevent chart from growing to infinity (consuming browser memory)
            if (dataset.data.length > MAX_SAMPLES) {
                wifiChart.data.labels = wifiChart.data.labels.slice(-MAX_SAMPLES);
                dataset.data = dataset.data.slice(-MAX_SAMPLES);
            }
            dataset.data.push(data.wifiSignal);
        });

        wifiChart.update();
    });

    batteryChart.data.labels = new Array(batteryData.length);
    batteryChart.data.datasets[0].data = batteryData;
    batteryChart.update();

    cutterLoadChart.data.labels = new Array(cutterLoadData.length);
    cutterLoadChart.data.datasets[0].data = cutterLoadData;
    cutterLoadChart.update();
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

    batteryChart = new Chart(document.getElementById("battery-chart"), {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                fill: true,
                lineTension: 0, // no bezierCurve
                backgroundColor: '#0B0',
                borderColor: '#070',
                data: [],                
            }]
        },
        options: {
            //animation: false,
            responsive: true,
            title: {
                display: true,
                text: 'Battery voltage'
            },
            legend: {
                display: false
            },
            scales: {
                yAxes: [{
                    display: true,
                    gridLines: {
                        color: '#777'
                    },
                    ticks: {
                        min: 0,
                        max: 20
                    }
                }]
            }
        }
    });

    wifiChart = new Chart(document.getElementById("wifi-chart"), {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                fill: false,
                lineTension: 0, // no bezierCurve
                backgroundColor: '#00B',
                borderColor: '#007',
                data: [],                
            }]
        },
        options: {
            //animation: false,
            responsive: true,
            title: {
                display: true,
                text: 'WiFi strength'
            },
            legend: {
                display: false
            },
            scales: {
                yAxes: [{
                    display: true,
                    gridLines: {
                        color: '#777'
                    },
                    //type: 'logarithmic',
                    /*ticks: {
                        min: -90,
                        max: -30
                    }*/
                }]
            }
        }
    });

    cutterLoadChart = new Chart(document.getElementById("cutterload-chart"), {
        type: 'line',
        data: {
            labels: [],
            datasets: [{
                fill: true,
                lineTension: 0, // no bezierCurve
                backgroundColor: '#00B',
                borderColor: '#007',
                data: [],                
            }]
        },
        options: {
            //animation: false,
            responsive: true,
            title: {
                display: true,
                text: 'Cutter load'
            },
            legend: {
                display: false
            },
            scales: {
                yAxes: [{
                    display: true,
                    gridLines: {
                        color: '#777'
                    },
                    ticks: {
                        min: 0,
                        max: 100
                    }
                }]
            }
        }
    });
}