import * as api from '../rest.js';
let sec = $('.js-section-settings');

export function selected() {

}

export function unselected() {

}

function restart() {
    api.restart()
    .fail(function() {
        alert('error');
    });
}

function factoryreset() {
    api.factoryreset()
    .fail(function() {
        alert('error');
    });
}

export function init() {
    sec.find('.js-restart').on('click', function() {
      restart();
    });
    sec.find('.js-factoryreset').on('click', function() {
        factoryreset();
    });
}