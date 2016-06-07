#include <pebble.h>
#include <pebble-events/pebble-events.h>

#include "owm-weather.h"

static OWMWeatherInfo *s_info;
static OWMWeatherCallback *s_callback;
static OWMWeatherStatus s_status;

static char s_api_key[33];

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *reply_tuple = dict_find(iter, MESSAGE_KEY_Reply);
  if(reply_tuple) {
    Tuple *desc_tuple = dict_find(iter, MESSAGE_KEY_Description);
    strncpy(s_info->description, desc_tuple->value->cstring, OWM_WEATHER_BUFFER_SIZE);

    Tuple *desc_short_tuple = dict_find(iter, MESSAGE_KEY_DescriptionShort);
    strncpy(s_info->description_short, desc_short_tuple->value->cstring, OWM_WEATHER_BUFFER_SIZE);

    Tuple *name_tuple = dict_find(iter, MESSAGE_KEY_Name);
    strncpy(s_info->name, name_tuple->value->cstring, OWM_WEATHER_BUFFER_SIZE);

    Tuple *temp_tuple = dict_find(iter, MESSAGE_KEY_TempK);
    s_info->temp_k = temp_tuple->value->int32;
    s_info->temp_c = s_info->temp_k - 273;
    s_info->temp_f = ((s_info->temp_c * 9) / 5 /* *1.8 or 9/5 */) + 32;
    s_info->timestamp = time(NULL);

    Tuple *pressure_tuple = dict_find(iter, MESSAGE_KEY_Pressure);
    s_info->pressure = pressure_tuple->value->int32;

    Tuple *wind_speed_tuple = dict_find(iter, MESSAGE_KEY_WindSpeed);
    s_info->wind_speed = wind_speed_tuple->value->int32;

    Tuple *wind_direction_tuple = dict_find(iter, MESSAGE_KEY_WindDirection);
    s_info->wind_direction = wind_direction_tuple->value->int32;

    s_status = OWMWeatherStatusAvailable;
    app_message_deregister_callbacks();
    s_callback(s_info, s_status);
  }

  Tuple *err_tuple = dict_find(iter, MESSAGE_KEY_BadKey);
  if(err_tuple) {
    s_status = OWMWeatherStatusBadKey;
    s_callback(s_info, s_status);
  }

  err_tuple = dict_find(iter, MESSAGE_KEY_LocationUnavailable);
  if(err_tuple) {
    s_status = OWMWeatherStatusLocationUnavailable;
    s_callback(s_info, s_status);
  }
}

static void fail_and_callback() {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to send request!");
  s_status = OWMWeatherStatusFailed;
  s_callback(s_info, s_status);
}

static bool fetch() {
  DictionaryIterator *out;
  AppMessageResult result = app_message_outbox_begin(&out);
  if(result != APP_MSG_OK) {
    fail_and_callback();
    return false;
  }

  dict_write_cstring(out, MESSAGE_KEY_Request, s_api_key);

  result = app_message_outbox_send();
  if(result != APP_MSG_OK) {
    fail_and_callback();
    return false;
  }

  s_status = OWMWeatherStatusPending;
  s_callback(s_info, s_status);
  return true;
}

void owm_weather_init(char *api_key) {
  if(s_info) {
    free(s_info);
  }

  if(!api_key) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "API key was NULL!");
    return;
  }

  strncpy(s_api_key, api_key, sizeof(s_api_key));

  s_info = (OWMWeatherInfo*)malloc(sizeof(OWMWeatherInfo));
  s_status = OWMWeatherStatusNotYetFetched;
  events_app_message_request_inbox_size(2026);
  events_app_message_request_outbox_size(656);
  events_app_message_register_inbox_received(inbox_received_handler, NULL);
}

bool owm_weather_fetch(OWMWeatherCallback *callback) {
  if(!s_info) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "OWM Weather library is not initialized!");
    return false;
  }

  if(!callback) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "OWMWeatherCallback was NULL!");
    return false;
  }

  s_callback = callback;

  if(!bluetooth_connection_service_peek()) {
    s_status = OWMWeatherStatusBluetoothDisconnected;
    s_callback(s_info, s_status);
    return false;
  }

  return fetch();
}

void owm_weather_deinit() {
  if(s_info) {
    free(s_info);
    s_info = NULL;
    s_callback = NULL;
  }
}

OWMWeatherInfo* owm_weather_peek() {
  if(!s_info) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "OWM Weather library is not initialized!");
    return NULL;
  }

  return s_info;
}
