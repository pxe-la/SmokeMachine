/* Utils */
export const getEl = (el) => document.getElementById(el);

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
export const createStore = () => {
  const _stateSubs = {}

  const state = new Proxy({}, {
    set(target, prop, val) {
      if (_stateSubs[prop]) {
        _stateSubs[prop].forEach((cb) => cb(val));
      }
      target[prop] = val;
      return true;
    },
  });

  const onChange = (key, cb) => {
    if (!_stateSubs[key]) _stateSubs[key] = [];
    _stateSubs[key].push(cb);
  }

  return {
    state,
    onChange,
  };
}


export const pick = (obj, props) => {
  if (!props) return Object.assign({}, obj);
  return props.reduce((acc, prop) => {
    if (obj[prop]) acc[prop] = obj[prop];
    return acc;
  }, {})
};

/* Queries */
export const GET = (url) => {
  return new Promise((resolve) => {
    const xhttp = new XMLHttpRequest();
    xhttp.onload = () => {
      if (xhttp.status == 200) {
        resolve(JSON.parse(xhttp.responseText));
      }
    };
    xhttp.open("GET", url, true);
    xhttp.send();
  });
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
