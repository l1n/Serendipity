#ifndef ARROW__H
#define ARROW__H
#include <pebble.h>
#define PI 3.1415932383

static GPath *arrow_ptr = NULL;
extern double arrow_rotation;

static const GPathInfo ARROW_PATH_INFO = {
  .num_points = 4,
  .points = (GPoint []) {{0, -60}, {18, 60}, {0, 30},  {-18, 60}}
};

void arrow_update_proc(Layer *arrow_layer, GContext* ctx);
void setup_arrow(void);
void destroy_arrow(void);

#endif
