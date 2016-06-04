# owm-weather

Library for easy fetching of weather data from [OpenWeatherMap.org](http://home.openweathermap.org).

Includes a simple test app as a proof of concept usage of a weather C API.

![basalt](screenshots/basalt.png)


## How to use

* Obtain an API key from [OpenWeatherMap.org](http://home.openweathermap.org/users/sign_up).

* Ensure `'enableMultiJS': true` is set in your appinfo.json.

* Run `pebble package install pebble-owm-weather`.

* Call `owm_weather_init(api_key)` to initialize the library when your app starts, supplying your API key.

* Call `owm_weather_fetch()` after PebbleKit JS is ready, and supply a suitable
  callback for events.

That's it! When the fetch returns (successful or not), the callback will be called with a `OWMWeatherInfo` object for you to extract data from.

## Documentation

Read `include/owm-weather.h` for function and `enum` documentation.

## Data returned

**Available now**

* Description, short description, temperature in K/C/F, location name.

**Not implemented, but possible**

Anything else detailed in the JSON response.
