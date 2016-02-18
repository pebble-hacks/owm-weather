var OWMWeather = function(options) {
  this._apiKey = '';

  options = options || {};
  this._appKeyBase = options.baseAppKey || 0;

  this._appKeys = {
    'Request': 0,
    'Reply': 1,
    'Description': 2,
    'DescriptionShort': 3,
    'Name': 4,
    'TempK': 5,
    'Pressure': 6,
    'WindSpeed': 7,
    'WindDirection': 8,
    'BadKey': 9,
    'LocationUnavailable': 10
  };

  this.getAppKey = function(keyName) {
    return this._appKeyBase + this._appKeys[keyName];
  };

  this.sendAppMessage = function(obj, onSuccess, onError) {
    var msg = {};
    for(var key in obj) {
      // Make sure the key exists
      if (!key in this._appKeys) throw 'Unknown key: ' + key;

      msg[this.getAppKey(key)] = obj[key];
    }

    Pebble.sendAppMessage(msg, onSuccess, onError);
  };

  this._xhrWrapper = function(url, type, callback) {
    var xhr = new XMLHttpRequest();
    xhr.onload = function () {
      callback(xhr);
    };
    xhr.open(type, url);
    xhr.send();
  };

  this.sendToPebble = function(json) {
    this.sendAppMessage({
      'Reply': 1,
      'Description': json.weather[0].description,
      'DescriptionShort': json.weather[0].main,
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
        this.sendAppMessage({ 'BadKey': 1 });
      }
    }.bind(this));
  };

  this._onLocationError = function(err) {
    console.log('owm-weather: Location error');
    this.sendAppMessage({
      'LocationUnavailable': 1
    });
  };

  this.appMessageHandler = function(dict) {
    if(dict.payload[this.getAppKey('Request')]) {
      this._apiKey = dict.payload[this.getAppKey('Request')];
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