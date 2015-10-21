#include "weather.h"

typedef enum {
  WeatherAppMessageKeyRequest = 0,
  WeatherAppMessageKeyReply,
  WeatherAppMessageKeyDescription,
  WeatherAppMessageKeyDescriptionShort,
  WeatherAppMessageKeyName,
  WeatherAppMessageKeyTempK
} WeatherAppMessageKey;

static WeatherInfo *s_info;
static WeatherCallback *s_callback;
static WeatherStatus s_status;

static char s_api_key[33];

static void inbox_received_handler(DictionaryIterator *iter, void *context) {
  Tuple *reply_tuple = dict_find(iter, WeatherAppMessageKeyReply);
  if(reply_tuple) {
    Tuple *desc_tuple = dict_find(iter, WeatherAppMessageKeyDescription);
    strncpy(s_info->description, desc_tuple->value->cstring, WEATHER_BUFFER_SIZE);

    Tuple *desc_short_tuple = dict_find(iter, WeatherAppMessageKeyDescriptionShort);
    strncpy(s_info->description_short, desc_short_tuple->value->cstring, WEATHER_BUFFER_SIZE);

    Tuple *temp_tuple = dict_find(iter, WeatherAppMessageKeyTempK);
    s_info->temp_k = temp_tuple->value->int32;
    s_info->temp_c = s_info->temp_k - 273; 
    s_info->temp_f = ((s_info->temp_c * 9) / 5 /* *1.8 or 9/5 */) + 32;
    s_info->timestamp = time(NULL);

    s_status = WeatherStatusAvailable;
    app_message_deregister_callbacks();
    s_callback(s_info, s_status);
  }
}

static void fail_and_callback() {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to send request!");
  s_status = WeatherStatusFailed;
  s_callback(s_info, s_status);
}

static bool fetch() {
  DictionaryIterator *out;
  AppMessageResult result = app_message_outbox_begin(&out);
  if(result != APP_MSG_OK) {
    fail_and_callback();
    return false;
  }

  dict_write_cstring(out, WeatherAppMessageKeyRequest, s_api_key);

  result = app_message_outbox_send();
  if(result != APP_MSG_OK) {
    fail_and_callback();
    return false;
  }

  s_status = WeatherStatusPending;
  s_callback(s_info, s_status);
  return true;
}

void weather_init(char *api_key) {
  if(s_info) {
    free(s_info);
  }

  if(!api_key) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "API key was NULL!");
    return;
  }

  strncpy(s_api_key, api_key, sizeof(s_api_key));

  s_info = (WeatherInfo*)malloc(sizeof(WeatherInfo));
  s_status = WeatherStatusNotYetFetched;
}

bool weather_fetch(WeatherCallback *callback) {
  if(!s_info) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Weather library is not initialized!");
    return false;
  }

  if(!callback) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "WeatherCallback was NULL!");
    return false;
  }

  s_callback = callback;

  if(!bluetooth_connection_service_peek()) {
    s_status = WeatherStatusBluetoothDisconnected;
    s_callback(s_info, s_status);
    return false;
  }

  app_message_deregister_callbacks();
  app_message_register_inbox_received(inbox_received_handler);
  app_message_open(2026, 656);

  return fetch();
}

void weather_deinit() {
  if(s_info) {
    free(s_info);
    s_info = NULL;
    s_callback = NULL;
  }
}

WeatherInfo* weather_peek() {
  if(!s_info) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "Weather library is not initialized!");
    return NULL;
  }

  return s_info;
}
