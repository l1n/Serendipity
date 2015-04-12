#ifndef COMMUNICATIONS__H
#define COMMUNICATIONS__H

#include <stdint.h>

void
communications_init(void);

void
communications_send_keypress(int key);

void
communications_set_bearing_callback(void (*cb)(double));

void
communications_set_destination_callback(void (*cb)(char const *));

#endif
