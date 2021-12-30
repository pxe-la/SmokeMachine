const dialogs = [
  {
    title: 'Auto Runner settings',
    url: '/api/state/',
    fields: {
      autoRunnerInterval: { label: 'Interval (ms)', type: 'number' },
      autoRunnerDuration: { label: 'Duration (ms)', type: 'number' },
    },
  },
  {
    title: 'Artnet settings',
    fields: {
      artNetUniverse: { label: 'Universe', type: 'number' },
      artNetChannel: { label: 'Channel', type: 'number' },
    },
  },
  {
    title: 'WiFi settings',
    url: '/api/wifi/',
    fields: {
      ssid: { label: 'SSID', maxlength: 31 },
      pass: { label: 'Password', maxlength: 31 },
    },
  },
  {
    title: 'AP settings',
    url: '/api/ap/',
    fields: {
      ssid: { label: 'SSID', maxlength: 31 },
      pass: { label: 'Password', maxlength: 31 },
    },
  },
  {
    title: 'Sticky time',
    url: '/api/state/',
    fields: {
      stickyTime: { label: 'Sticky time (ms)', type: 'number' },
    },
  },
];

dialogs.forEach((conf) => {
  const uuid = Math.random().toString(36).substr(2);
  const dialog = document.createElement('dialog');
  dialog.classList.add('mdl-dialog');
  dialog.id = `${uuid}-dialog`;
  dialog.innerHTML = `
    <p class="mdl-dialog__title">${conf.title}</p>
    <div class="mdl-dialog__content">
      <form id="${uuid}-form">
        ${Object.entries(fields).map(([fieldName, { label, ...restFieldProps}]) => `
          <div class="mdl-textfield mdl-js-textfield mdl-textfield--floating-label">
            <input class="mdl-textfield__input" type="number" name="${fieldName}" ${
              Object.entries(restFieldProps).map(([propName, propVal]) => `${propName}="${propVal}"` )
            }>
            <label class="mdl-textfield__label" for="autoRunnerInterval">${label}</label>
          </div>
        `)}
      <form>
    </div>
    <div class="mdl-dialog__actions">
      <button type="button" class="mdl-button mdl-js-button mdl-button--colored" form="${uuid}-form">Save</button>
      <button type="button" class="mdl-button close">Cancel</button>
    </div>`;
  document.body.appendChild(dialog);

  const navItem = document.createElement('a');
  navItem.classList.add('mdl-navigation__link');
  navItem.innerText = conf.title;
  navItem.onclick = dialog.showModal();
})
