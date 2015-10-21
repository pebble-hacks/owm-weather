#pragma once

#include <pebble.h>

#define WEATHER_BUFFER_SIZE 32

//! Possible statuses of the weather library
typedef enum {
  //! Weather library has not yet initiated a fetch
  WeatherStatusNotYetFetched = 0,

  //! Bluetooth is disconnected
  WeatherStatusBluetoothDisconnected,

  //! Weather data fetch is in progress
  WeatherStatusPending,

  //! Weather fetch failed
  WeatherStatusFailed,

  //! Weather fetched and available
  WeatherStatusAvailable
} WeatherStatus;

//! Struct containing weather data
typedef struct {
  //! Weather conditions string e.g: "Sky is clear"
  char description[WEATHER_BUFFER_SIZE];

  //! Short conditions string e.g: "Clear"
  char description_short[WEATHER_BUFFER_SIZE];

  //! Name of the location from the weather feed
  char name[WEATHER_BUFFER_SIZE];

  //! Temperature in degrees Kelvin, Celcius, and Farenheit
  int temp_k;
  int temp_c;
  int temp_f;

  //! Date that the data was received
  time_t timestamp;
} WeatherInfo;

//! Callback for a weather fetch
//! @param info The struct containing the weather data
//! @param status The current WeatherStatus, which may have changed.
typedef void(WeatherCallback)(WeatherInfo *info, WeatherStatus status);

//! Initialize the weather library. The data is fetched after calling this, and should be accessed
//! and stored once the callback returns data, if it is successful.
//! @param api_key The API key or 'appid' from your OpenWeatherMap account.
void weather_init(char *api_key);

//! Important: This uses the AppMessage system. You should only use AppMessage yourself
//! either before calling this, or after you have obtained your weather data.
//! @param callback Callback to be called once the weather.
//! @return true if the fetch message to PebbleKit JS was successful, false otherwise.
bool weather_fetch(WeatherCallback *callback);

//! Deinitialize and free the backing WeatherInfo.
void weather_deinit();

//! Peek at the current state of the weather library. You should check the WeatherStatus of the 
//! returned WeatherInfo before accessing data members.
//! @return WeatherInfo object, internally allocated. If NULL, weather_init() has not been called.
WeatherInfo* weather_peek();
