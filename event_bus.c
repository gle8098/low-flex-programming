#include "event_bus.h"
#include "panic.h"

typedef struct {
    int event_type;
    callback_t callback;
    void* arg;
} callback_entry_t;

#define CALLBACKS_MAX_COUNT 1024

static callback_entry_t known_callbacks[CALLBACKS_MAX_COUNT] = {};
static size_t callbacks_count = 0;

inline int is_empty(callback_entry_t* entry) {
    return entry->callback == NULL;
}

int register_event_callback(int event_type, callback_t callback, void* arg) {
    if (callbacks_count >= CALLBACKS_MAX_COUNT) {
        panic("Failed to register %d callback because max callbacks are register", event_type);
        return -1;
    }

    callback_entry_t cbck = {
            .event_type = event_type,
            .callback = callback,
            .arg = arg
    };
    known_callbacks[callbacks_count] = cbck;
    ++callbacks_count;
    return 0;
}

void remove_callback(size_t i) {
    ++i;
    for (; i < callbacks_count; ++i) {
        known_callbacks[i - 1] = known_callbacks[i];
    }
}

int unregister_event_callback(callback_t callback, void* arg) {
    for (size_t i = 0; i < callbacks_count; ++i) {
        callback_entry_t* entry = known_callbacks + i;
        if (entry->callback == callback && entry->arg == arg) {
            remove_callback(i);
            return 0;
        }
    }
    return -1;
}

int fire_event(event_t* event) {
    for (size_t i = 0; i < callbacks_count; ++i) {
        callback_entry_t* entry = known_callbacks + i;
        if (entry->event_type == event->header.event_type) {
            entry->callback(event, entry->arg);
        }
    }
    return 0;
}
