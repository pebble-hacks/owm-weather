#include <pebble.h>

#include "owm_weather/owm_weather.h"

static Window *s_window;
static TextLayer *s_text_layer;

static void weather_callback(OWMWeatherInfo *info, OWMWeatherStatus status) {
  switch(status) {
    case OWMWeatherStatusAvailable:
    {
      static char s_buffer[256];
      snprintf(s_buffer, sizeof(s_buffer),
        "Temperature (K/C/F): %d/%d/%d\n\nDescription/short:\n%s/%s\n\nPressure: %d\n\nWind speed/dir: %d/%d",
        info->temp_k, info->temp_c, info->temp_f, info->description,
        info->description_short, info->pressure, info->wind_speed, info->wind_direction);
      text_layer_set_text(s_text_layer, s_buffer);
    }
      break;
    case OWMWeatherStatusNotYetFetched:
      text_layer_set_text(s_text_layer, "OWMWeatherStatusNotYetFetched");
      break;
    case OWMWeatherStatusBluetoothDisconnected:
      text_layer_set_text(s_text_layer, "OWMWeatherStatusBluetoothDisconnected");
      break;
    case OWMWeatherStatusPending:
      text_layer_set_text(s_text_layer, "OWMWeatherStatusPending");
      break;
    case OWMWeatherStatusFailed:
      text_layer_set_text(s_text_layer, "OWMWeatherStatusFailed");
      break;
    case OWMWeatherStatusBadKey:
      text_layer_set_text(s_text_layer, "OWMWeatherStatusBadKey");
      break;
    case OWMWeatherStatusLocationUnavailable:
      text_layer_set_text(s_text_layer, "OWMWeatherStatusLocationUnavailable");
      break;
  }
}

static void js_ready_handler(void *context) {
  owm_weather_fetch(weather_callback);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_text_layer = text_layer_create(PBL_IF_ROUND_ELSE(
    grect_inset(bounds, GEdgeInsets(20, 0, 0, 0)),
    bounds));
  text_layer_set_text(s_text_layer, "Ready.");
  text_layer_set_text_alignment(s_text_layer, PBL_IF_ROUND_ELSE(GTextAlignmentCenter, GTextAlignmentLeft));
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

  // Replace this with your own API key from OpenWeatherMap.org
  char *api_key = "12341234123412341234123412341234";
  owm_weather_init(api_key);

  app_timer_register(3000, js_ready_handler, NULL);
}

static void deinit() {
  owm_weather_deinit();
}

int main() {
  init();
  app_event_loop();
  deinit();
}
