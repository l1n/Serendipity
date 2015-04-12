#include "communications.h"
#include <pebble.h>

typedef enum msg_send {
  MSG_KEYPRESS = 0
} msg_send;
typedef enum msg_recv {
  MSG_BEARING = 0,
  MSG_DEBUG = 1,
  MSG_DESTINATION = 2
} msg_recv;

void
communications_send_keypress(int key)
{
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  dict_write_int(iter, MSG_KEYPRESS, &key, sizeof (int), true);
  app_message_outbox_send();
}

void (*bearing_callback)(double) = NULL;

void
communications_set_bearing_callback(void (*cb)(double))
{
  bearing_callback = cb;
}

void (*destination_callback)(char const *);

void
communications_set_destination_callback(void (*cb)(char const *))
{
  destination_callback = cb;
}

extern TextLayer* debug_layer;

static void
inbox_received_callback(DictionaryIterator *iterator,
                        void *context)
{
  Tuple *t = dict_read_first(iterator);
  static char debug[128], destination[128];
  while (t != NULL) {
    switch (t->key) {
    case MSG_BEARING:
      if (!bearing_callback) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "No bearing callback");
        break;
      }
      if (t->type != TUPLE_BYTE_ARRAY || t->length != 4) {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Invalid bearing message");
        break;
      }
      bearing_callback((double) *((int32_t *) t->value) / 1000000.0);
      break;
    case MSG_DEBUG:
      strncpy(debug, t->value->cstring, 127);
      text_layer_set_text(debug_layer, debug);
      break;
    case MSG_DESTINATION:
      strncpy(destination, t->value->cstring, 127);
      if (destination_callback) destination_callback(destination);
      break;
    }
    t = dict_read_next(iterator);
  }
}

static void
inbox_dropped_callback(AppMessageResult reason,
                       void *context)
{
  // APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void
outbox_failed_callback(DictionaryIterator *iterator,
                       AppMessageResult reason,
                       void *context)
{
  // APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void
outbox_sent_callback(DictionaryIterator *iterator,
                     void *context)
{
  // APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

void
communications_init(void)
{
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
}

