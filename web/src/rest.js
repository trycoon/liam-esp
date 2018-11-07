$.ajaxSetup({
  cache: false,
  timeout: 5000 // 5 seconds
});
  
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

export function getLogmessages() {
  return $.getJSON('/api/v1/logmessages');
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
