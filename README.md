# weather

Weather library for easy fetching of weather data from OpenWeatherMap.org.


## How to use

* Obtain an API key from [OpenWeatherMap.org](http://openweathermap.org/api).

* Add these `AppMessage` keys to the `appKeys` array in `appinfo.json`:

```
"WeatherAppMessageKeyRequest": 0,
"WeatherAppMessageKeyReply": 1,
"WeatherAppMessageKeyDescription": 2,
"WeatherAppMessageKeyDescriptionShort": 3,
"WeatherAppMessageKeyName": 4,
"WeatherAppMessageKeyTempK": 5
```

* Insert `weather/weather.js` into your apps' `pebble-js-app.js`.

* Call `weatherHandle()` in said `appmessage` handler so that it can message the C side.

* Copy the `weather` directory into your project's `src` directory and include
  the library in your C files that will use it:

```
#include "weather.h"
``` 

* Call `weather_init()` to initialize the library when your app starts.

* Call `weather_fetch()`,  after PebbleKit JS is ready supplying a suitable
  callback for events.

That's it! When the fetch returns (successful or not), the callback will be
called with a `WeatherInfo` object for you to extract data from.


## Documentation

Read `weather/weather.h` for function and `enum` documentation.


## Data returned

**Available now**

* Description, short description, temperature in K/C/F, location name.

**Not implemented, but possible**

Anything else detailed in the JSON response.

