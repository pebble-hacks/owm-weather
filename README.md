# owm-weather

Library for easy fetching of weather data from [OpenWeatherMap.org](http://home.openweathermap.org).

Includes a simple test app as a proof of concept usage of a weather C API.

![basalt](screenshots/basalt.png)


## How to use

* Obtain an API key from [OpenWeatherMap.org](http://home.openweathermap.org/users/sign_up).

* Ensure `'enableMultiJS': true` is set in your appinfo.json.

* Add `owm_weather/owm_weather.js` to `/src/js/lib`, include it in your your app.js file, and then instantiate an OWMWeather object.

```
var OWMWeather = require('lib/owm_weather.js');
var owm = new OWMWeather();
```

* Call `owm.appMessageHandler()` in an `appmessage` handler so that it can message the C side.

```
Pebble.addEventListener('appmessage', function(e) {
  owm.appMessageHandler(e);
});
```

* Copy the `owm_weather` directory into your project's `src` directory and include the library in any C files that will use it:

```
#include "owm_weather/owm_weather.h"
```

* Call `owm_weather_init(api_key)` to initialize the library when your app starts, supplying your API key.

* Call `owm_weather_fetch()`, after PebbleKit JS is ready supplying a suitable
  callback for events.

That's it! When the fetch returns (successful or not), the callback will be called with a `OWMWeatherInfo` object for you to extract data from.

### OWMWeather AppMessage Keys

By default, the OWMWeather library requires 11 consecutive unused App Message Keys (and defaults to 0 - 10). If you wish to use a different set of consecutive keys, you can set the AppKey base by changing the JavaScript and C initializers to:

```
var OWMWeather = require('lib/owm_weather.js');

// Use App Keys 10-20
var owm = new OWMWeather({ appKeyBase: 10 });
```

```
// Use App Keys 10-20
owm_weather_init_with_base_app_key(apiKey, 10);
```

**NOTE:** You *must* set the AppKey base in both the C and JS application code to use this feature.

## Documentation

Read `owm_weather/owm_weather.h` for function and `enum` documentation.


## Data returned

**Available now**

* Description, short description, temperature in K/C/F, location name.

**Not implemented, but possible**

Anything else detailed in the JSON response.
