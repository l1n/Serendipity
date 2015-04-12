#include "debug.h"

TextLayer *debug_layer;

void setupLayer(void) {
  debug_layer = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { 144, 14 } });
}

void destroyLayer(void) {
  text_layer_destroy(debug_layer);
}
