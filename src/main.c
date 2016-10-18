#include <pebble.h>

#include <pebble-events/pebble-events.h>
#include "enamel.h"

static Window *window;
static TextLayer *text_layer_title;
static TextLayer *text_layer_food;
static TextLayer *text_layer_drink;
static Layer *bottom_layer;

static EventHandle* s_evt_handler;

static char favorite_food_string[100];

static void updateDisplay(void* context) {
  if(enamel_get_enable_background()) {
    window_set_background_color(window, enamel_get_background());
  }
  else {
    window_set_background_color(window,GColorWhite);
  }

  APP_LOG(0, "fontsize is %d", enamel_get_font_size());

  switch(enamel_get_font_size()){
    case FONT_SIZE_SMALL : text_layer_set_font(text_layer_title, fonts_get_system_font(FONT_KEY_GOTHIC_14)); break;
    case FONT_SIZE_NORMAL : text_layer_set_font(text_layer_title, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD)); break;
    case FONT_SIZE_LARGE : text_layer_set_font(text_layer_title, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD)); break;
  }

  text_layer_set_text(text_layer_drink, enamel_get_favorite_drink());

  APP_LOG(0, "email is %s", enamel_get_email());
  APP_LOG(0, "flavor is %s", enamel_get_flavor());

  layer_mark_dirty(bottom_layer);
  
}

void update_proc(Layer *layer, GContext *ctx){
  GRect bounds = layer_get_bounds(layer);

  bounds.origin.y = bounds.size.h - 30;
  bounds.size.h = 30;
  bounds.size.w = enamel_get_slider() * bounds.size.w / (SLIDER_PRECISION * 100);

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, bounds, 0, 0);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer_title = text_layer_create(GRect(0,0,bounds.size.w,30));
  text_layer_set_text(text_layer_title, "Enamel Demo");
  text_layer_set_background_color(text_layer_title, GColorClear);
  text_layer_set_text_alignment(text_layer_title, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer_title));

  text_layer_food = text_layer_create(GRect(0,40,bounds.size.w,100));
  text_layer_set_text(text_layer_food, favorite_food_string);
  text_layer_set_background_color(text_layer_food, GColorClear);
  text_layer_set_text_alignment(text_layer_food, GTextAlignmentLeft);
  layer_add_child(window_layer, text_layer_get_layer(text_layer_food));

  text_layer_drink = text_layer_create(GRect(0,40,bounds.size.w,100));
  text_layer_set_text(text_layer_drink, favorite_food_string);
  text_layer_set_background_color(text_layer_drink, GColorClear);
  text_layer_set_text_alignment(text_layer_drink, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(text_layer_drink));

  bottom_layer = layer_create(bounds);
  layer_set_update_proc(bottom_layer, update_proc);
  layer_add_child(window_layer, bottom_layer);

  updateDisplay(NULL);
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer_title);
  text_layer_destroy(text_layer_food);
  text_layer_destroy(text_layer_drink);
  layer_destroy(bottom_layer);
}

static void init(void) {
  // Initialize Enamel to register App Message handlers and restores settings
  enamel_init();

  // Register our custom receive handler
  s_evt_handler = enamel_settings_received_subscribe(updateDisplay,NULL);

  // call pebble-events app_message_open function
  events_app_message_open(); 

  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
}

static void deinit(void) {
  window_destroy(window);
  
  // Deinit Enamel to unregister App Message handlers and save settings
  enamel_settings_received_unsubscribe(s_evt_handler);
  enamel_deinit();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
