  export function selectState(state) {
    return $.ajax({
      url: '/api/v1/state',
      method: 'PUT',
      headers: {
        'content-type': 'application/json',
        'cache-control': 'no-cache'
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

 export function manual(command, params) {
    return $.ajax({
      url: `/api/v1/manual/${command}`,
      method: 'PUT',
      headers: {
        'content-type': 'application/json',
        'cache-control': 'no-cache'
      },
      data: params ? JSON.stringify(params) : '{}'
    });
  }
