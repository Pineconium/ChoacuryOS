#pragma once

#include "mouse.h"
#include "types.h"

typedef struct {
    bool enabled;
    u8 id;
    u8 button_mask;
} ps2_mouse_info_t;

typedef struct ps2_device ps2_device_t;

void ps2_init_mouse(ps2_device_t*);

/* Write mouse event to out. If no event has happened, out->type == MOUSE_EVENT_NONE */
void ps2_get_mouse_event(mouse_event_t* out);
