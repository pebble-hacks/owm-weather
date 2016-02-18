var OWMWeather = function() {

  this.owmWeatherAPIKey = '';

  this.owmWeatherXHR = function(url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
      callback(this.responseText);
    };
    xhr.open(type, url);
    xhr.send();
  }

  this.owmWeatherSendToPebble = function(json) {
    Pebble.sendAppMessage({
      'OWMWeatherAppMessageKeyReply': 1,
      'OWMWeatherAppMessageKeyDescription': json.weather[0].description,
      'OWMWeatherAppMessageKeyDescriptionShort': json.weather[0].main,
      'OWMWeatherAppMessageKeyTempK': Math.round(json.main.temp),
      'OWMWeatherAppMessageKeyName': json.name,
      'OWMWeatherAppMessageKeyPressure': Math.round(json.main.pressure),
      'OWMWeatherAppMessageKeyWindSpeed': Math.round(json.wind.speed),
      'OWMWeatherAppMessageKeyWindDirection': Math.round(json.wind.deg)
    });
  }

  this.owmWeatherLocationSuccess = function(pos) {
    var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + this.owmWeatherAPIKey;
    console.log('owm-weather: Location success. Contacting OpenWeatherMap.org...');

    this.owmWeatherXHR(url, 'GET', function(responseText) {
      console.log('owm-weather: Got API response!');
      if(responseText.length > 100) {
        this.owmWeatherSendToPebble(JSON.parse(responseText));
      } else {
        console.log('owm-weather: API response was bad. Wrong API key?');
        Pebble.sendAppMessage({ 'OWMWeatherAppMessageKeyBadKey': 1 });
      }
    }.bind(this));
  }

  this.owmWeatherLocationError = function(err) {
    console.log('owm-weather: Location error');
    Pebble.sendAppMessage({
      'OWMWeatherAppMessageKeyLocationUnavailable': 1
    });
  }

  this.appMessageHandler = function(dict) {
    if(dict.payload['OWMWeatherAppMessageKeyRequest']) {
      this.owmWeatherAPIKey = dict.payload['OWMWeatherAppMessageKeyRequest'];
      console.log('owm-weather: Got fetch request from C app');

      navigator.geolocation.getCurrentPosition(this.owmWeatherLocationSuccess.bind(this), this.owmWeatherLocationError.bind(this), {
        timeout: 15000,
        maximumAge: 60000
      });
    }

    return true;
  };
}

module.exports = OWMWeather;