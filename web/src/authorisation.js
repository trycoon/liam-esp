import * as api from './rest.js';
let submitHandler, loginPromise;

/**
 * show login dialog, returns a promise that resolves when login is successful.
 */
export function showLogin() {
    if (loginPromise) {
        return loginPromise;
    } else {
        loginPromise = new Promise((resolve) => {
            let loginBox = document.querySelector('.js-loginbox');
            let username = loginBox.querySelector('.js-username');
            let password = loginBox.querySelector('.js-password');
            let submit = loginBox.querySelector('.js-submit');
            let messageBox = loginBox.querySelector('.js-message');
            messageBox.textContent = '';
            loginBox.style.display = 'block';
    
            document.querySelector('.js-no-connection-modal').style.display = 'none';
            document.getElementById('modal').style.display = 'block';
            username.focus();
    
            // make sure we remove old eventlisteners first so we don't trigger twice.
            if (typeof submitHandler === 'function') {
                submit.removeEventListener('click', submitHandler);
            }
    
            submitHandler = (evt) => {
                loginBox.classList.remove('shake');
    
                api.createSession(username.value, password.value)
                .done(result => {
                    document.getElementById('modal').style.display = 'none';
                    loginBox.style.display = 'none';
                    messageBox.textContent = '';
                    loginPromise = null; // allow future logins.
                    resolve(result);
                })
                .catch(error => {
                    if (error.status === 401) {
                        password.value = '';
                        messageBox.textContent = 'username or password incorrect';
                        loginBox.classList.add('shake');
                    } else {
                        messageBox.textContent = 'unknown error, retry later';
                        loginBox.classList.add('shake');
                        console.error('Failed to login. error: ' + error.message);
                    }
                })
                evt.preventDefault();
            }
    
            submit.addEventListener('click', submitHandler);
        });

        return loginPromise;
    }
}

export function isLoginDialogVisible() {
    return !!loginPromise;
}