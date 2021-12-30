import { GET, POST, on, getEl, createStore } from './utils.js';

const store = createStore();

let stickyTime = null;

const updateState = async () => {
  const result = await GET('/api/state');
  Object.assign(store.state, result);
};
setInterval(updateState, 2000);
updateState();


store.onChange('temp', (val) => getEl('temp').innerText = val);
store.onChange('autoRunner', (val) => {
  const checkbox = getEl('autoRunner');
  checkbox.checked = val;
  checkbox.parentNode.MaterialSwitch.checkToggleState();
});
on('autoRunner', 'change', (e) => {
  store.state.autoRunner = e.target.checked;
  POST('/api/autoRunner', { enabled: e.target.checked });
})


function debounce(f, t){
  let tId;
  return (...args) => {
    clearTimeout(tId);
    tId = setTimeout(() => { f.apply(this, args); }, t);
  };
}

let resendIntervalId = null;
let pressStart = false;
let clicks = 0;

const btnRun_press = (e) => {
  clicks++;
  if (clicks == 1) {
    pressStart = true;
    if(resendIntervalId) clearInterval(resendIntervalId);
    const foo = () => POST("/api/run", { dur: 1500 });
    resendIntervalId = setInterval(foo, 1000);
    foo();
  }
}

on('btn-run', 'mousedown', btnRun_press);
on('btn-run', 'touchstart', btnRun_press);

const btnRun_release = debounce((e) => {
  if (!pressStart) return;
  pressStart = false;
  if (resendIntervalId) clearInterval(resendIntervalId);
  resendIntervalId = null;
  console.log(clicks);
  switch(clicks) {
    case 1:
      POST('/api/run'); // without params means stop
      break;
    case 2:
      POST('/api/run', { dur: store.state.stickyTime });
      break;
    case 3:
      store.state.recStickyTime = !store.state.recStickyTime;
      POST('/api/stickyTime', {recStickyTime: store.state.recStickyTime});
      break;
  }
  clicks = 0;
}, 300);
on('mouseup', btnRun_release);
on('touchend', btnRun_release);


getEl('drawerNav').addEventListener('click', async (e) => {
  const dialog = getEl(e.target.dataset.dialog)
  const form = dialog.querySelector('form');
  const data = await GET(form.dataset.api);
  dialog.showModal();
  Object.entries(data).forEach(([key, val]) => {
    const input = form.elements[key];
    if (!input) return;
    input.value = val;
    input.parentNode.MaterialTextfield.checkDirty();
  })
})

document.querySelectorAll('dialog').forEach((dialog) => {
  const closeBtn = dialog.querySelector('.close');
  closeBtn.onclick = () => dialog.close();

  const form = dialog.querySelector('form');
  form.onsubmit = (e) => {
    e.preventDefault();
    POST(form.dataset.api, Object.fromEntries((new FormData(e.target)).entries()));
    dialog.close();
  }
})
