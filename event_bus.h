#pragma once

#include <stddef.h>

#define EVENT_KEY_PRESS 1

struct event_header {
    int event_type;
    size_t length;
};
typedef struct event_header event_header_t;

struct event {
    event_header_t header;
    char data[0];
};
typedef struct event event_t;

typedef void (*callback_t)(event_t* event, void* arg);

int register_event_callback(int event_type, callback_t callback, void* arg);
int unregister_event_callback(callback_t callback, void* arg);
int fire_event(event_t* event);
