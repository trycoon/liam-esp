let uptime = new Date(),
    loglevel = 4,
    currentState = {
        state: 'DOCKED',
        isCharging: false,
        cutterRotating: false,
        batteryVoltage: Math.floor(Math.random() * (16.8 - 14.0 + 1)) + 14.0,
        batteryLevel: Math.round(Math.random() * 100),
        lastFullyChargeTime: new Date() - 1000,
        lastChargeDuration: 1000 * 60 * 60,
        wifiSignal: Math.floor(Math.random() * (-30 - -90 + 1)) + -90,
        cutterLoad: Math.round(Math.random() * 100),
        uptime: uptime,
        leftWheelSpd: 80,
        rightWheelSpd: 80,
        pitch: 0,
        roll: 0,
        heading: 0,
        targetHeading: 45,
    },
    currentSystem = {
        name: 'liam-esp',
        version: '1.0.0',        
        cpuFreq: 240,
        flashChipSize: 4194304,
        freeHeap: 109608,
        apiKey: '17ff5eEc39fD847B',
        settings: {
            batteryFullVoltage: 16.8,
            batteryEmptyVoltage: 14.0,
        }
    };

  module.exports = {
    getCurrentState: () => {
        let state = JSON.parse(JSON.stringify(currentState));   // deep copy, don't affect original.
        state.batteryVoltage = Math.floor(Math.random() * (16.8 - 14.0 + 1)) + 14.0;
        state.batteryLevel  = Math.round(Math.random() * 100);
        state.cutterLoad = Math.round(Math.random() * 100);
        state.heading = Math.floor(Math.random() * 90) - 45;
        state.uptime = Math.round((new Date().getTime() - uptime.getTime()) / 1000)
        state.wifiSignal = Math.floor(Math.random() * (-30 - -90 + 1)) + -90;

        return state;
    },
    getCurrentSystem: () => {
        let system = JSON.parse(JSON.stringify(currentSystem));   // deep copy, don't affect original.

        return system;
    },
    getBatterySamples: () => {
        let samples = [],
        date = new Date().getTime();

        for (let i = 0; i < 100; i++) {
            samples.push({
                t: date + i,
                v: 14.8 - i / 10
            });
        }

        return samples;
    },
    cutterOn: () => {
        currentState.cutterRotating = true;
    },
    cutterOff: () => {
        currentState.cutterRotating = false;
    },
    setState: (state) => {
        currentState.state = state;
    },
    getLoglevel: () => {
        return {
            level: loglevel
        };
    },
    setLoglevel: (level) => {
        loglevel = level;
    },
    getLogmessages: () => {
        return {
            messages: 'N: Gyro/accelerometer/compass init success.\nN: New state: DOCKED\nN: AP Started, AP SSID: "liam-esp", AP IPv4: 192.168.4.1\nN: Web server initialized\nN: using MQTT server IP: 192.168.1.10, and port: 1883\nN: OTA available.\nN: Connected to WiFi accesspoint "Stargate", using IP-address: 192.168.1.123 and MAC: 30:AE:A4:19:33:44\nN: Connected to the MQTT broker.'
        }
    },
    setApiKey: (key) => {
        currentSystem.apiKey = key;
    }
  }
  