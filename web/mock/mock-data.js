const moveSequence = require('./moveSequence.json'),
      timeRegExp = /(00|01|02|03|04|05|06|07|08|09|10|11|12|13|14|15|16|17|18|19|20|21|22|23):(0|1|2|3|4|5)\d/g;

let uptime = new Date(),
    loglevel = 4,
    seqPos = 0,
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
      leftWheelSpd: 0,
      rightWheelSpd: 0,
      pitch: 0,
      roll: 0,
      heading: 0,
    },
    currentSystem = {
      name: 'liam-esp',
      version: '1.0.0',
      mowerId: '413445680',       
      cpuFreq: 240,
      flashChipSize: 4194304,
      chipRevision: 1,
      freeHeap: 109608,
      apiKey: '17ff5eEc39fD847B',
      localTime: '01 Nov 2018, 21:57:10+0100',
      settings: {
        batteryFullVoltage: 16.8,
        batteryEmptyVoltage: 14.0,
      },
    },
    schedules = [];

module.exports = {
  getCurrentState: () => {
    let state = JSON.parse(JSON.stringify(currentState));   // deep copy, don't affect original.
    state.batteryVoltage = Math.floor(Math.random() * (16.8 - 14.0 + 1)) + 14.0;
    state.batteryLevel  = Math.round(Math.random() * 100);
    state.cutterLoad = Math.round(Math.random() * 100);
    state.heading = Math.floor(Math.random() * 90) - 45;

    let currSequence = moveSequence[seqPos];
    state.roll = currSequence.r || 0;
    state.pitch = currSequence.p || 0;
    state.leftWheelSpd = currSequence.lw || 0;
    state.rightWheelSpd = currSequence.rw || currSequence.lw;
    
    state.uptime = Math.round((new Date().getTime() - uptime.getTime()) / 1000);
    state.wifiSignal = Math.floor(Math.random() * (-30 - -90 + 1)) + -90;
    
    // random movement and rotation if needed.
    /*state.roll = Math.floor(Math.random() * 90) - 45;
    state.pitch = Math.floor(Math.random() * 90) - 45;
    state.leftWheelSpd = Math.round(-100 + Math.random() * (100 - -100));  // -100 to 100
    state.rightWheelSpd = Math.round(-100 + Math.random() * (100 - -100)); // -100 to 100*/
    seqPos = (seqPos + 1) % moveSequence.length;
    console.log(`current movesequence: ${seqPos}`);
    
    return state;
  },
  getCurrentSystem: () => {
    let system = JSON.parse(JSON.stringify(currentSystem));   // deep copy, don't affect original.

    return system;
  },
  getBatterySamples: () => {
    let samples = {
          time: [],
          value: [],
        },
        date = new Date().getTime();

    for (let i = 0; i < 100; i++) {
      samples.time.push(date + i);
      samples.value.push(16.8 - i / 100);
    }

    return samples;
  },
  cutterOn: () => {
    currentState.state = 'MANUAL';
    currentState.cutterRotating = true;
  },
  cutterOff: () => {
    currentState.state = 'MANUAL';
    currentState.cutterRotating = false;
  },
  forward: (speed, turnrate, smooth) => {
    currentState.state = 'MANUAL';
  },
  backward: (speed, turnrate, smooth) => {
    currentState.state = 'MANUAL';
  },
  stop: () => {
    currentState.state = 'MANUAL';
    currentState.leftWheelSpd = 0;
    currentState.rightWheelSpd = 0;
  },
  setState: (state) => {
    currentState.state = state;
  },
  getLoglevel: () => {
    return {
      level: loglevel,
    };
  },
  setLoglevel: (level) => {
    loglevel = level;
  },
  getLogmessages: () => {
    return {
      messages: [
        'N: Gyro/accelerometer/compass init success.',
        'N: New state: DOCKED',
        'N: AP Started, AP SSID: "liam-esp", AP IPv4: 192.168.4.1',
        'N: Web server initialized',
        'N: using MQTT server IP: 192.168.1.10, and port: 1883',
        'N: OTA available.',
        'N: Connected to WiFi accesspoint "Stargate", using IP-address: 192.168.1.123 and MAC: 30:AE:A4:19:33:44',
        'N: Connected to the MQTT broker.',
      ],
    };
  },
  setApiKey: (key) => {
    currentSystem.apiKey = key;
  },
  getSchedules: () => {
    return schedules;
  },
  addScheduleEntry: (entry) => {    
    if (entry.startTime.match(timeRegExp) && entry.stopTime.match(timeRegExp)) {
      schedules.unshift({ 
        activeWeekdays: entry.activeWeekdays,
        startTime: entry.startTime,
        stopTime: entry.stopTime,
      });
    }
  },
  removeScheduleEntry: (position) => {
    schedules.splice(position, 1);
  },
};
  