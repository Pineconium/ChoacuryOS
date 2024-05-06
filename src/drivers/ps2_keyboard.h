#pragma once

#include "key.h"

#include <stdbool.h>

typedef struct {
   key_event_t key_event;
} ps2_key_event_t;

typedef struct {
    u8 modifiers;
} ps2_keyboard_info;

typedef struct ps2_device_t ps2_device_t;

extern key_t ps2_keymap_normal[0xFF];
extern key_t ps2_keymap_shift[0xFF];
extern key_t ps2_keymap_extended[0xFF];

void ps2_init_keyboard(ps2_device_t*);
void ps2_keyboard_new_byte(ps2_device_t*);

/* Write key event to out. If no event has happened, key is KEY_NONE */
void ps2_get_key_event(key_event_t* out);
