#pragma once

#include "ps2_keyboard.h"
#include "ps2_mouse.h"

#define PS2_DUMP_FULL_EVENT_QUEUE 0

#define PS2_TYPE_NONE     0
#define PS2_TYPE_KEYBOARD 1
#define PS2_TYPE_MOUSE    2

#define PS2_COMMAND_QUEUE_SIZE 32
#define PS2_BYTE_BUFFER_SIZE 32
#define PS2_EVENT_QUEUE_SIZE 32

#define PS2_DEVICE_COMMAND_IDENTIFY         0xF2
#define PS2_DEVICE_COMMAND_ENABLE_SCANNING  0xF4
#define PS2_DEVICE_COMMAND_DISABLE_SCANNING 0xF5
#define PS2_DEVICE_COMMAND_RESET            0xFF

typedef struct ps2_device {
    u8 type;
    void (*callback)(struct ps2_device*);

    u8 byte_buffer_len;
    u8 byte_buffer[PS2_BYTE_BUFFER_SIZE];

    u8 event_queue_len;
    u8 event_queue_head;
    u8 event_queue_tail;
    union {
        struct {
            key_event_t keyboard_event_queue[PS2_EVENT_QUEUE_SIZE];
            ps2_keyboard_info_t keyboard_info;
        };
        struct {
            mouse_event_t mouse_event_queue[PS2_EVENT_QUEUE_SIZE];
            ps2_mouse_info_t mouse_info;
        };
    };
} ps2_device_t;

void ps2_init();
void ps2_device_append_command_queue(ps2_device_t* device, u8 command, u8 response_size);
void ps2_device_append_command_queue_with_data(ps2_device_t* device, u8 command, u8 data, u8 response_size);
ps2_device_t* ps2_get_device(u8 index);
