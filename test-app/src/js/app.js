var OWMWeather = require('pebble-owm-weather');

var owmWeather = new OWMWeather();

Pebble.addEventListener('ready', function(e) {
  console.log('PebbleKit JS ready!');
});

Pebble.addEventListener('appmessage', function(e) {
  console.log('appmessage: ' + JSON.stringify(e.payload));
  owmWeather.appMessageHandler(e);
});
