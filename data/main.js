import { GETtoState, POSTfromState, POST, on, state, bindState } from './utils.js';

bindState('temp', 'temp', 'innerText');
bindState('stickyTime');

let stickyTime = null;

const updateState = () => GETtoState('/api/state', ['temp', 'stickyTime', 'recStickyTime']);
setInterval(updateState, 2000);
updateState();

function debounce(f, t){
  let tId;
  return (...args) => {
    clearTimeout(tId);
    tId = setTimeout(() => { f.apply(this, args); }, t);
  };
}

let resendIntervalId = null;

const btnRun_press = (e) => {
  if (e.detail == 1) {
    clearInterval(resendIntervalId);
    const foo = () => POST("/api/run", { dur: 1500 });
    resendIntervalId = setInterval(foo, 1000);
    foo();
  }
}

on('btn-run', 'mousedown', btnRun_press);
on('btn-run', 'touchstart', btnRun_press);

const btnRun_release = debounce((e) => {
  clearInterval(resendIntervalId);
  switch(e.detail) {
    case 1:
      POST('/api/run'); // without params means stop
    break;
    case 2:
      POST("/api/run", { dur: state.stickyTime });
      break;
    case 3:
      state.recStickyTime = !state.recStickyTime;
      POSTfromState("/api/state", ['recStickyTime']);
      break;
  }
}, 300);
on('mouseup', btnRun_release);
on('touchend', btnRun_release);

on('settings-form', 'submit', (e) => {
  e.preventDefault();
  POST('/api/state', {stickyTime: state.stickyTime});
})
