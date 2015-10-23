/*************************** Weather library start ****************************/

var weatherAPIKey = '';

function weatherXHR(url, type, callback) {
  var xhr = new XMLHttpRequest();
  xhr.onload = function () {
    callback(this.responseText);
  };
  xhr.open(type, url);
  xhr.send();
}

function weatherSendToPebble(json) {
  Pebble.sendAppMessage({
    'WeatherAppMessageKeyReply': 1,
    'WeatherAppMessageKeyDescription': json.weather[0].description,
    'WeatherAppMessageKeyDescriptionShort': json.weather[0].main,
    'WeatherAppMessageKeyTempK': json.main.temp,
    'WeatherAppMessageKeyName': json.name
  }, function(e) {
    console.log('weather: Sent data to Pebble!');
  }, function(e) {
    console.log('weather: Error sending data to Pebble');
  });
}

function weatherLocationSuccess(pos) {
  var url = 'http://api.openweathermap.org/data/2.5/weather?lat=' + 
    pos.coords.latitude + '&lon=' + pos.coords.longitude + '&appid=' + weatherAPIKey;
  console.log('weather: Location success. Contacting OpenWeatherMap.org...');

  weatherXHR(url, 'GET', function(responseText) {
    console.log('weather: Got API response!');
    if(responseText.length > 100) {
      weatherSendToPebble(JSON.parse(responseText));
    } else {
      console.log('weather: API response was bad. Wrong API key?');
      // TODO Inform C of bad API key
    }
  });
}

function weatherLocationError(err) {
  console.log('weather: Location error');
  // TODO Inform C of location unavailable
}

function weatherHandle(dict) {
  if(dict.payload['WeatherAppMessageKeyRequest']) {
    weatherAPIKey = dict.payload['WeatherAppMessageKeyRequest'];
    console.log('weather: Got fetch request from C app');
    
    navigator.geolocation.getCurrentPosition(weatherLocationSuccess, weatherLocationError, {
      timeout: 15000,
      maximumAge: 60000
    });
  }
};

/**************************** Weather library end *****************************/