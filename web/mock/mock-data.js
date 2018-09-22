let uptime = new Date(),
    loglevel = 4,
    currentState = {
        state: 'DOCKED',
        isCharging: false,
        cutterRotating: false,
        batteryVoltage: Math.floor(Math.random() * (10 - 16 + 1)) + 16,
        batteryLevel: Math.random() * 100,
        lastFullyChargeTime: new Date() - 1000,
        lastChargeDuration: 1000 *60 * 60,
        cutterLoad: Math.random() * 100,
        leftWheelSpd: 80,
        rightWheelSpd: 80,
        pitch: 0,
        roll: 0,
        heading: 0,
        targetHeading: 45
    },
    currentSystem = {
        name: 'liam-esp',
        version: '1.0.0',
        uptime: uptime,
        cpuFreq: 240,
        flashChipSize: 4194304,
        freeHeap: 109608,
        wifiSignal: Math.floor(Math.random() * (-30 - -90 + 1)) + -90
    };

  module.exports = {
    getCurrentState: () => {
        let state = JSON.parse(JSON.stringify(currentState));   // deep copy, don't affect original.
        state.batteryVoltage = Math.floor(Math.random() * (10 - 16 + 1)) + 16;
        state.batteryLevel  = Math.random() * 100;
        state.cutterLoad = Math.random() * 100;
        state.heading = Math.floor(Math.random() * 90) - 45;

        return state;
    },
    getCurrentSystem: () => {
        let system = JSON.parse(JSON.stringify(currentSystem));   // deep copy, don't affect original.
        system.uptime = Math.round((new Date().getTime() - uptime.getTime()) / 1000)
        system.wifiSignal = Math.floor(Math.random() * (-30 - -90 + 1)) + -90;
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
    }
  }
  