liam.rest =
(function() {

  function selectState(state) {
    return $.ajax({
      url: '/api/v1/state',
      method: "PUT",
      data: {
        state: state
      }
    });
  }

  function getStatus(params) {
    return $.getJSON('/api/v1/status', params);
  }

  function getSystem(params) {
    return $.getJSON('/api/v1/system', params);
  }

  return {
    selectState: selectState,
    getStatus: getStatus,
    getSystem: getSystem
  }
});
