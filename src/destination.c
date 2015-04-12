#include "destination.h"
#include <pebble.h>
#include "communications.h"

TextLayer *destination_layer;

static void
callback(char const *destination)
{
  static char msg[128];
  snprintf(msg, 127, "Arrived: %s", destination);
  text_layer_set_text(destination_layer, msg);
  layer_set_hidden(text_layer_get_layer(destination_layer), false);
}

void
destination_init(void)
{
  GRect rect = { .origin = { 0, 77 }, .size = { 144, 14 } };
  destination_layer = text_layer_create(rect);
  text_layer_set_text_alignment(destination_layer, GTextAlignmentCenter);
  layer_set_hidden(text_layer_get_layer(destination_layer), true);
  communications_set_destination_callback(callback);
}

void
destination_deinit(void)
{
  text_layer_destroy(destination_layer);
}
