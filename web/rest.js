liam.rest =
(function() {
  'use strict';

  function selectState(state) {
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

  function getStatus(params) {
    return $.getJSON('/api/v1/status', params);
  }

  function getSystem(params) {
    return $.getJSON('/api/v1/system', params);
  }

  function manual(command, params) {
    return $.ajax({
      url: `/api/v1/manual/${command}`,
      method: 'PUT',
      headers: {
        'content-type': 'application/json',
        'cache-control': 'no-cache'
      },
      data: params || '{}'
    });
  }

  return {
    selectState: selectState,
    getStatus: getStatus,
    getSystem: getSystem,
    manual: manual
  }
})();
