#include "arrow.h"

double arrow_rotation = 0;

void arrow_update_proc(Layer *arrow_layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  gpath_draw_filled(ctx, arrow_ptr);
  gpath_move_to(arrow_ptr, GPoint(72, 84));
  gpath_rotate_to(arrow_ptr, (double) TRIG_MAX_ANGLE / 2 / PI * (arrow_rotation - PI / 4));
}

void setup_arrow(void) {
  arrow_ptr = gpath_create(&ARROW_PATH_INFO);
}

void destroy_arrow(void) {
  gpath_destroy(arrow_ptr);
}
