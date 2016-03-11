#include <pebble.h>

#include "enamel.h"

static Window *window;
static TextLayer *text_layer_title;
static TextLayer *text_layer_food;
static TextLayer *text_layer_drink;

static char favorite_food_string[100];

static char* mystrcat( char* dest, const char* src )
{
  while (*dest) dest++;
  while ((*dest++ = *src++));
  return --dest;
}

static void updateDisplay() {
  if(get_enable_background()) {
    window_set_background_color(window, get_background());
  }
  else {
    window_set_background_color(window,GColorWhite);
  }

  switch(get_font_size()){
    case FONT_SIZE_SMALL : text_layer_set_font(text_layer_title, fonts_get_system_font(FONT_KEY_GOTHIC_14)); break;
    case FONT_SIZE_NORMAL : text_layer_set_font(text_layer_title, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD)); break;
    case FONT_SIZE_LARGE : text_layer_set_font(text_layer_title, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD)); break;
  }

  favorite_food_string[0] = '\0';
  char *p = favorite_food_string;
  for(uint16_t i=0; i<get_favorite_food_count(); i++){
    p = mystrcat(p,get_favorite_food(i));
    p = mystrcat(p,"\n");
  }
  text_layer_set_text(text_layer_food, favorite_food_string);

  text_layer_set_text(text_layer_drink, get_favorite_drink());

  APP_LOG(0, "email is %s", get_email());
  
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  updateDisplay();
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

  updateDisplay();
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer_title);
  text_layer_destroy(text_layer_food);
  text_layer_destroy(text_layer_drink);
}

static void init(void) {
  // Initialize Enamel to register App Message handlers and restores settings
  enamel_init(0, 0);

  // Register our custom receive handler
  enamel_register_custom_inbox_received(in_received_handler);

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
  enamel_deinit();
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
