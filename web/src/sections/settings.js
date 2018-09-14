import * as api from '../rest.js';
let sec = $('.js-section-settings');

export function selected() {
    api.getLoglevel()
    .then(function(level) {
        $(`#loglever option[value="${level}"]`).prop('selected', true)
    })
    .fail(function() {
        console.error(e);
    });
}

export function unselected() {

}

function restart() {
    api.restart()
    .fail(function() {
        console.error(e);
    });
}

function factoryreset() {
    api.factoryreset()
    .fail(function() {
        console.error(e);
    });
}

export function init() {
    sec.find('.js-restart').on('click', function() {
      restart();
    });
    sec.find('.js-factoryreset').on('click', function() {
        if (confirm('Are you sure you want to wipe all settings and return mower to "factory" defaults?')) {
            factoryreset();
        }
    });
    sec.find('#loglever').change(function() {
        api.setLoglevel(parseInt(this.value))
        .then(function() {
            alert("You must reboot system for loglevel changes to be used.")
        })
        .fail(function() {
            console.error(e);
        });
    });
}