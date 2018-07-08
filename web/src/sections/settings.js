import * as api from '../rest.js';
let sec = $('.js-section-settings');

export function selected() {

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
        factoryreset();
    });
}