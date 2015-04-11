#include "orientation.h"
#include <pebble.h>

void (*callback)(double) = NULL;

void
orientation_set_callback(void (*cb)(double))
{
  callback = cb;
}

static void
handler(CompassHeadingData heading)
{
  if (!callback) return;
  callback((double) heading.true_heading / TRIG_MAX_ANGLE * 2 * 3.141592653);
}

void
orientation_init(void)
{
  compass_service_set_heading_filter(0);
  compass_service_subscribe(handler);
}
  
