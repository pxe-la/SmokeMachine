/* Utils */
export const getEl = (el) => document.getElementById(el);

export const pick = (obj, props) => props.reduce((acc, prop) => {
  if (obj[prop]) acc[prop] = obj[prop];
  return acc;
}, {});

/* Events */
const _subs = {};
export const on = (id, evName, handler) => {
  // значит подписались на глобальный event
  if (!handler) {
    handler = evName;
    evName = id;
    id = null;
  }
  if (!(evName in _subs)) {
    _subs[evName] = [];
    document.addEventListener(evName, (evt) =>
      _subs[evName].forEach((sub) =>
        (!sub.id || evt.target.id === sub.id) && sub.handler(evt)),
    );
  }
  _subs[evName].push({ id, handler });
}

/* State management and binding */
const _stateSubs = {}

export const state = new Proxy({}, {
  set(target, prop, val) {
    if (_stateSubs[prop]) {
      _stateSubs[prop].forEach(({ el, prop: elProp }) => {
        el[elProp] = val;
      });
    }
    target[prop] = val;
    return true;
  },
});

export const bindState = (stateProp, el, prop, evName) => {
  if (!el) el = stateProp;
  if (!prop) prop = 'value';
  if (!evName) evName = 'input';
  if (typeof el === 'string') el = getEl(el);
  if (evName) {
    on(el.id, evName, (evt) => {
      state[stateProp] = evt.target[prop];
    })
  }
  if (!_stateSubs[stateProp]) _stateSubs[stateProp] = [];
  _stateSubs[stateProp].push({ el, prop });
}

/* Queries */
export const GETtoState = (url, props) => {
  const xhttp = new XMLHttpRequest();
  xhttp.onload = () => {
    if (xhttp.status == 200) {
      Object.assign(state, pick(JSON.parse(xhttp.responseText), props));
    }
  };
  xhttp.open("GET", url, true);
  xhttp.send();
}

const encodeUriObject = (obj) => Object.entries(obj)
  .map(([key, value]) => (
    encodeURIComponent(key) + "=" + encodeURIComponent(value)
  ))
  .join('&');

export const POST = (url, data={}) => {
  const xhttp = new XMLHttpRequest();
  xhttp.open("POST", url, true);
  xhttp.setRequestHeader('Content-Type','application/x-www-form-urlencoded')
  xhttp.send(encodeUriObject(data));
}

export const POSTfromState = (url, props) => POST(url, pick(state, props));
