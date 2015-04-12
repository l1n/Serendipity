#include <pebble.h>
#include "communications.h"
#include "orientation.h"
#include "arrow.h"
#include "debug.h"
#include "destination.h"

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
  setupLayer();
  destination_init();

  layer_add_child(window_layer, compass_layer);
  layer_add_child(window_layer, text_layer_get_layer(debug_layer));
  layer_add_child(window_layer, text_layer_get_layer(destination_layer));
}

static void
window_unload(Window *window)
{
  destroy_arrow();
  destroyLayer();
  destination_deinit();
}

double bearing, orientation;

static void
calc_rotation()
{
  double _bearing = bearing, _orientation = orientation;
  if (_bearing > PI) _bearing -= 2*PI;
  if (_orientation > PI) _orientation -= 2*PI;
  arrow_rotation = _bearing + _orientation;
  if (arrow_rotation < 0) arrow_rotation += 2 * PI;
}

static void
recv_bearing(double b)
{
  bearing = b;
  static char str[16];
  snprintf(str, 15, "%d", (int) (b / PI * 360));
  text_layer_set_text(debug_layer, str);
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
