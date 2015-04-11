#include <pebble.h>
#include "communications.h"
#include "orientation.h"
#include "arrow.h"

Window *window;
TextLayer *text_layer;
TextLayer *overlay_layer;
Layer *compass_layer;

static void
select_click_handler(ClickRecognizerRef recognizer,
                     void *context)
{
  text_layer_set_text(text_layer, "Select");
  communications_send_keypress(0);
}

static void
up_click_handler(ClickRecognizerRef recognizer,
                 void *context)
{
  text_layer_set_text(text_layer, "Up");
  layer_mark_dirty(compass_layer);
  communications_send_keypress(1);
}

static void
down_click_handler(ClickRecognizerRef recognizer,
                   void *context)
{
  text_layer_set_text(text_layer, "Down");
  communications_send_keypress(-1);
}

static void
click_config_provider(void *context)
{
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void
window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press buttons");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);

  overlay_layer = text_layer_create((GRect) { .origin = { 0, 36 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(overlay_layer, "Izzie");
  text_layer_set_text_alignment(overlay_layer, GTextAlignmentCenter);

  compass_layer = layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, bounds.size.h } });
  layer_set_clips(compass_layer, true);
  layer_set_update_proc(compass_layer, arrow_update_proc);

  // layer_add_child(window_layer, text_layer_get_layer(overlay_layer));
  // layer_add_child(window_layer, text_layer_get_layer(text_layer));
  layer_add_child(window_layer, compass_layer);
}

static void
window_unload(Window *window)
{
  text_layer_destroy(text_layer);
  text_layer_destroy(overlay_layer);
  layer_destroy(compass_layer);
}

double bearing, orientation;

static void
recv_bearing(double b)
{
  APP_LOG(APP_LOG_LEVEL_DEBUG, "BEARING: %d", (int) (b*1000));
  bearing = b;
  arrow_rotation = bearing - orientation;
  layer_mark_dirty(compass_layer);
}

static void
recv_orientation(double o)
{
  orientation = o;
  arrow_rotation = bearing - orientation;
  layer_mark_dirty(compass_layer);
}

static void
init(void)
{
  setup_arrow();

  communications_init();
  communications_set_bearing_callback(recv_bearing);

  orientation_init();
  orientation_set_callback(recv_orientation);


  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  WindowHandlers winhandlers = { .load = window_load, .unload = window_unload };
  window_set_window_handlers(window, winhandlers);

  const bool animated = true;
  window_stack_push(window, animated);
}

static void
deinit(void)
{
  gpath_destroy(arrow_ptr);
  window_destroy(window);
}

int
main(void)
{
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
