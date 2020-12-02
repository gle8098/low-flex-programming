#pragma once

#include "event_bus.h"

struct keyboard_event {
    event_header_t header;
    int keycode;
};
typedef struct keyboard_event keyboard_event_t;

void turn_on_capslock_light();
