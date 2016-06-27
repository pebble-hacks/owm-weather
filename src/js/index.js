var OWMWeather = function(options) {
  this._apiKey = '';

  options = options || {};

  this._xhrWrapper = function(url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
      callback(xhr);
    };
    xhr.open(type, url);
    xhr.send();
  };

  this.sendToPebble = function(json) {
    Pebble.sendAppMessage({
      'Reply': 1,
      'Description': json.weather[0].description,
      'DescriptionShort': json.weather[0].main,
      'Icon': json.weather[0].icon,
      'TempK': Math.round(json.main.temp),
      'Name': json.name,
      'Pressure': Math.round(json.main.pressure),
      'WindSpeed': Math.round(json.wind.speed),
      'WindDirection': Math.round(json.wind.deg)
    });
  };

  this._onLocationSuccess = function(pos) {
    var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' +
      pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + this._apiKey;
    console.log('owm-weather: Location success. Contacting OpenWeatherMap.org...');

    this._xhrWrapper(url, 'GET', function(req) {
      console.log('owm-weather: Got API response!');
      if(req.status == 200) {
        this.sendToPebble(JSON.parse(req.response));
      } else {
        console.log('owm-weather: Error fetching data (HTTP Status: ' + req.status + ')');
        Pebble.sendAppMessage({ 'BadKey': 1 });
      }
    }.bind(this));
  };

  this._onLocationError = function(err) {
    console.log('owm-weather: Location error');
    Pebble.sendAppMessage({
      'LocationUnavailable': 1
    });
  };

  this.appMessageHandler = function(dict) {
    if(dict.payload['Request']) {
      this._apiKey = dict.payload['Request'];
      console.log('owm-weather: Got fetch request from C app');

      navigator.geolocation.getCurrentPosition(
        this._onLocationSuccess.bind(this),
        this._onLocationError.bind(this), {
          timeout: 15000,
          maximumAge: 60000
      });
    }
  };
};

module.exports = OWMWeather;
