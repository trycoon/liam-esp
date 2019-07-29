import * as auth from './authorisation.js';
import ReconnectingWebSocket from 'reconnecting-websocket';

let socket,
    socketDisconnectedTimeout;

$.ajaxSetup({
  cache: false,
  timeout: 5000 // 5 seconds
});

function showLostConnectionModal() {
  document.querySelector('.js-no-connection-modal').style.display = 'block';
  document.querySelector('.js-loginbox').style.display = 'none';
  document.getElementById('modal').style.display = 'block';
}

function hideLostConnectionModal() {
  document.getElementById('modal').style.display = 'none';
  document.querySelector('.js-no-connection-modal').style.display = 'none';
  document.querySelector('.js-loginbox').style.display = 'none';
}

export function setupSocket() {
  if (!socket) {
    let protocol = location.protocol.indexOf('https') === 0 ? 'wss' : 'ws';
    socket = new ReconnectingWebSocket(`${protocol}://${location.host}/ws`);

    socket.addEventListener('open', () => {
      console.info('Got WS connection.');
      if (socketDisconnectedTimeout) {
        clearTimeout(socketDisconnectedTimeout);
        socketDisconnectedTimeout = undefined;
      }
      if (!auth.isLoginDialogVisible()) {
        hideLostConnectionModal();
      }
    });

    socket.addEventListener('close', () => {
      console.info('Lost WS connection.');
      // show lost connection modal if we have not been able to reconnect within 2 seconds.
      socketDisconnectedTimeout = setTimeout(() => {
        // if we are not currently logging in, show lost connection warning.
        if (!auth.isLoginDialogVisible()) {
          showLostConnectionModal();
        }
      }, 2000);    
    });

    socket.addEventListener('error', (error) => {
      console.warn(`Got WS error: ${error.message}`);
      showLostConnectionModal();
    });

    // Listen for messages
    socket.addEventListener('message', function (event) {
      let message = JSON.parse(event.data);

      if (message.type === 'status') {
        let status = message.payload;
        if (JSON.stringify(status) !== JSON.stringify(liam.data.status)) {
          liam.data.status = status;
          window.dispatchEvent(new Event('statusUpdated'));
        }
      }
    });
  }
}

export function socketSend(messageType, payload) {
  if (socket && socket.readyState === socket.OPEN) {
    socket.send(JSON.stringify({
      type: messageType,
      payload: payload
    }));
  }
}
  
export function selectState(state) {
  return $.ajax({
    url: '/api/v1/state',
    method: 'PUT',
    headers: {
      'content-type': 'application/json',
    },
    data: JSON.stringify({
      state: state
    })
  });
}

export function getStatus() {
  return $.getJSON('/api/v1/status');
}

export function getSystem() {
  return $.getJSON('/api/v1/system');
}

export function getBatteryHistory() {
  return $.getJSON('/api/v1/history/battery');
}

export function manual(command, params) {
  return $.ajax({
    url: `/api/v1/manual/${command}`,
    method: 'PUT',
    headers: {
      'content-type': 'application/json',
    },
    data: params ? JSON.stringify(params) : '{}'
  });
}

export function restart() {
  return $.ajax({
    url: `/api/v1/reboot`,
    method: 'PUT',
    headers: {
      'content-type': 'application/json',
    },
    data: '{}'
  });
}

export function factoryreset() {
  return $.ajax({
    url: `/api/v1/factoryreset`,
    method: 'PUT',
    headers: {
      'content-type': 'application/json',
    },
    data: '{}'
  });
}

export function generateNewApiKey() {
  return $.ajax({
    url: `/api/v1/apikey`,
    method: 'POST',
    headers: {
      'content-type': 'application/json',
    },
    data: '{}'
  });
}

export function getLoglevel() {
  return $.getJSON('/api/v1/loglevel');
}

export function getLogmessages(lastnr) {
  return $.getJSON(`/api/v1/logmessages?lastnr=${lastnr || 0}`);
}

export function setLoglevel(level) {
  return $.ajax({
    url: `/api/v1/loglevel`,
    method: 'PUT',
    headers: {
      'content-type': 'application/json',
    },
    data: JSON.stringify({
      level: level
    })
  });
}

export function createSession(username, password) {
  return $.ajax({
    url: `/api/v1/session`,
    method: 'POST',
    headers: {
      'content-type': 'application/json',
    },
    data: JSON.stringify({
      username: username,
      password: password
    })
  });
}

export function getSession() {
  return $.getJSON('/api/v1/session');
}

export function deleteSession() {
  return $.ajax({
    url: `/api/v1/session`,
    method: 'DELETE'    
  });
}

export function getScheduleList() {
  return $.getJSON('/api/v1/schedules');
}

/**
 * Add new schedule-entry for mowing
 * @param {Array} activeWeekdays array of boolean, where each position represent a weekday and wether that day is selected.
 * @param {String} startTime HH:MM of when mower should start mowing
 * @param {String} stopTime HH:MM of when mower should stop mowing} activeWeekdays 
 */
export function addScheduleEntry(activeWeekdays, startTime, stopTime) {
  return $.ajax({
    url: `/api/v1/schedules`,
    method: 'POST',
    headers: {
      'content-type': 'application/json',
    },
    data: JSON.stringify({
      activeWeekdays: activeWeekdays,
      startTime: startTime,
      stopTime: stopTime
    })
  });
}

/**
 * Delete schedule-entry for mowing
 * @param {Number} position
 */
export function removeScheduleEntry(position) {
  return $.ajax({
    url: `/api/v1/schedules/` + position,
    method: 'DELETE',
    headers: {
      'content-type': 'application/json',
    }
  });
}
