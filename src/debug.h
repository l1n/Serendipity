#ifndef DEBUG__H
#define DEBUG__H
#include <pebble.h>

extern TextLayer *debug_layer;

void setupLayer(void);
void destroyLayer(void);

#endif
