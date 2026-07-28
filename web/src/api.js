import * as auth from './authorisation.js';
import { WebSocket as PartySocket } from 'partysocket';
let socket,
    socketDisconnectedTimeout;

const REQUEST_TIMEOUT_MS = 5000;

async function request(url, method = 'GET', data) {
  const controller = new AbortController();
  const timeoutHandle = setTimeout(() => controller.abort(), REQUEST_TIMEOUT_MS);

  try {
    const response = await fetch(url, {
      method,
      cache: 'no-store',
      credentials: 'same-origin',
      headers: {
        'content-type': 'application/json',
      },
      body: data === undefined ? undefined : JSON.stringify(data),
      signal: controller.signal,
    });

    if (!response.ok) {
      const error = new Error(`Request failed (${response.status})`);
      error.status = response.status;
      throw error;
    }

    const contentType = response.headers.get('content-type') || '';
    if (contentType.includes('application/json')) {
      return response.json();
    }

    return null;
  } catch (error) {
    if (error.name === 'AbortError') {
      const timeoutError = new Error('Request timed out');
      timeoutError.status = 0;
      throw timeoutError;
    }
    throw error;
  } finally {
    clearTimeout(timeoutHandle);
  }
}

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
    socket = new PartySocket(`${protocol}://${location.host}/ws`, [], {
      minReconnectionDelay: 0,
      maxReconnectionDelay: 10000,
      reconnectionDelayGrowFactor: 1.3,
      minUptime: 5000,
      connectionTimeout: 4000,
    });

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
  return request('/api/v1/state', 'PUT', {
    state: state,
  });
}

export function getStatus() {
  return request('/api/v1/status');
}

export function getSystem() {
  return request('/api/v1/system');
}

export function getBatteryHistory() {
  return request('/api/v1/history/battery');
}

export function manual(command, params) {
  return request(`/api/v1/manual/${command}`, 'PUT', params || {});
}

export function restart() {
  return request('/api/v1/reboot', 'PUT', {});
}

export function factoryreset() {
  return request('/api/v1/factoryreset', 'PUT', {});
}

export function generateNewApiKey() {
  return request('/api/v1/apikey', 'POST', {});
}

export function getLoglevel() {
  return request('/api/v1/loglevel');
}

export function getLogmessages(lastnr) {
  return request(`/api/v1/logmessages?lastnr=${lastnr || 0}`);
}

export function setLoglevel(level) {
  return request('/api/v1/loglevel', 'PUT', {
    level: level,
  });
}

export function createSession(username, password) {
  return request('/api/v1/session', 'POST', {
    username: username,
    password: password,
  });
}

export function getSession() {
  return request('/api/v1/session');
}

export function deleteSession() {
  return request('/api/v1/session', 'DELETE');
}

export function getScheduleList() {
  return request('/api/v1/schedules');
}

/**
 * Add new schedule-entry for mowing
 * @param {Array} activeWeekdays array of boolean, where each position represent a weekday and wether that day is selected.
 * @param {String} startTime HH:MM of when mower should start mowing
 * @param {String} stopTime HH:MM of when mower should stop mowing} activeWeekdays 
 */
export function addScheduleEntry(activeWeekdays, startTime, stopTime) {
  return request('/api/v1/schedules', 'POST', {
    activeWeekdays: activeWeekdays,
    startTime: startTime,
    stopTime: stopTime,
  });
}

/**
 * Delete schedule-entry for mowing
 * @param {Number} position
 */
export function removeScheduleEntry(position) {
  return request(`/api/v1/schedules/${position}`, 'DELETE');
}
