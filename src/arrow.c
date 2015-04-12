#include "arrow.h"

GPathInfo ARROW_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{0, -60}, {18, 60}, {0, 30},  {-18, 60}}
};
GPath *arrow_ptr = NULL;

double arrow_rotation = 0;
Layer *compass_layer;

static void arrow_update_proc(Layer *arrow_layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  gpath_draw_filled(ctx, arrow_ptr);
  gpath_move_to(arrow_ptr, GPoint(72, 84));

  gpath_rotate_to(arrow_ptr, (double) TRIG_MAX_ANGLE / 2 / PI * (arrow_rotation));
  layer_mark_dirty(arrow_layer);
}

void setup_arrow(GRect bounds) {
  arrow_ptr = gpath_create(&ARROW_PATH_INFO);
  compass_layer = layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, bounds.size.h } });
  layer_set_clips(compass_layer, true);
  layer_set_update_proc(compass_layer, arrow_update_proc);
}

void destroy_arrow(void) {
  gpath_destroy(arrow_ptr);
  layer_destroy(compass_layer);
}
