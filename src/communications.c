#include "communications.h"
#include <pebble.h>

typedef enum msg_send {
  MSG_KEYPRESS = 0
} msg_send;
typedef enum msg_recv {
  MSG_BEARING = 0
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

static void
inbox_received_callback(DictionaryIterator *iterator,
                        void *context)
{
  Tuple *t = dict_read_first(iterator);
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

