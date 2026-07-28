import * as api from '../api.js';
import * as auth from '../authorisation.js';

const sec = document.querySelector('.js-section-settings');

export function selected() {
    api.getSystem()
    .then(function(data) {
        liam.data.system = data;
        sec.querySelector('#apikey').value = data.apiKey;
    })
    .catch(error => {
        if (error.status === 401) {
          auth.showLogin().then(() => {
            selected();
          });
        } else {
            console.error(error.message);
        }
    });

    api.getLoglevel()
    .then(function(data) {
        const option = sec.querySelector(`#loglever option[value="${data.level}"]`);
        if (option) {
            option.selected = true;
        }
    })
    .catch(function(error) {
        console.error(error.message);
    });
}

export function unselected() {

}

function restart() {
    api.restart()
    .catch(error => {
        if (error.status === 401) {
          auth.showLogin().then(() => {
            restart();
          });
        } else {
                        console.error(error.message);
        }
    });
}

function factoryreset() {
    api.factoryreset()
    .catch(error => {
        if (error.status === 401) {
          auth.showLogin().then(() => {
            factoryreset();
          });
        } else {
            console.error(error.message);
        }
    });
}

export function init() {
    sec.querySelector('.js-restart').addEventListener('click', function() {
      restart();
    });
    sec.querySelector('.js-factoryreset').addEventListener('click', function() {
        if (confirm('Are you sure you want to wipe ALL settings and return mower to "factory" defaults?')) {
            factoryreset();
        }
    });
    sec.querySelector('#loglever').addEventListener('change', function() {
        api.setLoglevel(parseInt(this.value))
        .then(function() {
            alert("You must reboot system for loglevel changes to be used.")
        })
        .catch(function(error) {
            console.error(error.message);
        });
    });
    sec.querySelector('.js-generateApiKey').addEventListener('click', function() {
        api.generateNewApiKey()
        .then(function() {
            api.getSystem()
            .then(function(data) {
                liam.data.system = data;
                sec.querySelector('#apikey').value = data.apiKey;
            })
            .catch(function(error) {
                console.error(error.message);
            });
        })
        .catch(function(error) {
            console.error(error.message);
        });
    });
}