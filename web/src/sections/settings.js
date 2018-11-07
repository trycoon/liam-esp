import * as api from '../rest.js';
let sec = $('.js-section-settings');

export function selected() {
    api.getSystem()
    .then(function(data) {
        liam.data.system = data;
        sec.find('#apikey').val(data.apiKey);
    })
    .fail(function(e) {
        console.error(e.message);
    });

    api.getLoglevel()
    .then(function(data) {
        $(`#loglever option[value="${data.level}"]`).prop('selected', true);
    })
    .fail(function(e) {
        console.error(e.message);
    });
}

export function unselected() {

}

function restart() {
    api.restart()
    .fail(function(e) {
        console.error(e.message);
    });
}

function factoryreset() {
    api.factoryreset()
    .fail(function(e) {
        console.error(e.message);
    });
}

export function init() {
    sec.find('.js-restart').on('click', function() {
      restart();
    });
    sec.find('.js-factoryreset').on('click', function() {
        if (confirm('Are you sure you want to wipe ALL settings and return mower to "factory" defaults?')) {
            factoryreset();
        }
    });
    sec.find('#loglever').change(function() {
        api.setLoglevel(parseInt(this.value))
        .then(function() {
            alert("You must reboot system for loglevel changes to be used.")
        })
        .fail(function(e) {
            console.error(e.message);
        });
    });
    sec.find('.js-generateApiKey').on('click', function() {
        api.generateNewApiKey()
        .then(function() {
            api.getSystem()
            .then(function(data) {
                liam.data.system = data;
                sec.find('#apikey').val(data.apiKey);
            })
            .fail(function(e) {
                console.error(e.message);
            });
        })
        .fail(function(e) {
            console.error(e.message);
        });
    });
}