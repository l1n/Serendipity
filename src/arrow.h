#ifndef ARROW__H
#define ARROW__H
#include <pebble.h>
#define PI 3.1415932383

extern Layer *compass_layer;
extern double arrow_rotation;

void setup_arrow(GRect bounds);
void destroy_arrow(void);

#endif
