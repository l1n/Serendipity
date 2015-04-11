#include <pebble.h>
#include "communications.h"
#include "orientation.h"
#include "arrow.h"

Window *window;

static void
select_click_handler(ClickRecognizerRef recognizer,
                     void *context)
{
  communications_send_keypress(0);
}

static void
up_click_handler(ClickRecognizerRef recognizer,
                 void *context)
{
  /* layer_mark_dirty(compass_layer); */
  communications_send_keypress(1);
}

static void
down_click_handler(ClickRecognizerRef recognizer,
                   void *context)
{
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

  setup_arrow(bounds);

  layer_add_child(window_layer, compass_layer);
}

static void
window_unload(Window *window)
{
  destroy_arrow();
}

double bearing, orientation;

static void
calc_rotation()
{
  arrow_rotation = orientation - bearing;
  if (arrow_rotation < 0) arrow_rotation += 2 * 3.141592653;
}

static void
recv_bearing(double b)
{
  bearing = b;
  calc_rotation();
  layer_mark_dirty(compass_layer);
}

static void
recv_orientation(double o)
{
  orientation = o;
  calc_rotation();
  layer_mark_dirty(compass_layer);
}

static void
init(void)
{
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
