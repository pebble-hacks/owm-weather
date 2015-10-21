#include <pebble.h>

#include "weather/weather.h"

static Window *s_window;
static TextLayer *s_text_layer;

static void weather_callback(WeatherInfo *info, WeatherStatus status) {
  switch(status) {
    case WeatherStatusAvailable:
    {
      static char s_time_buffer[8];
      struct tm *time_stamp_tm = localtime(&info->timestamp);
      strftime(s_time_buffer, sizeof(s_time_buffer), "%H:%M", time_stamp_tm);

      static char s_buffer[256];
      snprintf(s_buffer, sizeof(s_buffer),
        "Temperature (K/C/F): %d/%d/%d\n\nDescription:\n%s\n\nDescription (short):\n%s\n\nUpdated: %s",
        info->temp_k, info->temp_c, info->temp_f, info->description, 
        info->description_short, s_time_buffer);
      text_layer_set_text(s_text_layer, s_buffer);
    }
      break;
    case WeatherStatusNotYetFetched:
      text_layer_set_text(s_text_layer, "WeatherStatusNotYetFetched");
      break;
    case WeatherStatusBluetoothDisconnected:
      text_layer_set_text(s_text_layer, "WeatherStatusBluetoothDisconnected");
      break;
    case WeatherStatusPending:
      text_layer_set_text(s_text_layer, "WeatherStatusPending");
      break;
    case WeatherStatusFailed:
      text_layer_set_text(s_text_layer, "WeatherStatusFailed");
      break;
  }
}

static void fetch_handler(void *context) {
  weather_fetch(weather_callback);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(bounds);
  text_layer_set_text(s_text_layer, "Ready.");
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(s_text_layer);

  window_destroy(window);
}

static void init() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  weather_init(API_KEY);
  
  app_timer_register(3000, fetch_handler, NULL);
}

static void deinit() { 
  weather_deinit();
}

int main() {
  init();
  app_event_loop();
  deinit();
}
