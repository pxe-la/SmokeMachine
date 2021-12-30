const express = require('express');
const bodyParser = require('body-parser')
const app = express();

app.use(bodyParser.urlencoded({ extended: true }));

const autoRunner = {
  enabled: false,
  interval: 1000,
  duration: 1000,
}

app.use(express.static(__dirname + '/front'));
app.get("/api/autoRunner", (req, res) => {
  res.json(autoRunner);
});
app.post("/api/autoRunner", (req, res) => {
  if (req.body.enabled) req.body.enabled = req.body.enabled === 'true',
  console.log(req.body);

  Object.assign(autoRunner, req.body);
});

const wifi = {
  ssid: 'AlexNET',
  pass: '123456',
}
app.get("/api/wifi", (req, res) => {
  res.json(wifi);
});
app.post("/api/wifi", (req, res) => {
  console.log(req.body);
  Object.assign(wifi, req.body);
});

const ap = {
  ssid: 'AlexNET',
  pass: '123456',
}
app.get("/api/ap", (req, res) => {
  res.json(ap);
});
app.post("/api/ap", (req, res) => {
  console.log(req.body);
  Object.assign(ap, req.body);
});

const stickyTime = {
  stickyTime: 300,
  recStickyTime: false,
}
app.get("/api/stickyTime", (req, res) => {
  res.json(stickyTime);
});
app.post("/api/wifi", (req, res) => {
  console.log(req.body);
  Object.assign(stickyTime, req.body);
});

app.get("/api/state", (req, res) => {
  res.json({
    temp: Math.floor(Math.random() * 100 + 200),
    autoRunner: autoRunner.enabled,
  });
});

app.listen(3000);
