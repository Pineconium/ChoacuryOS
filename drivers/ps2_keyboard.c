#include "critical.h"
#include "debug.h"
#include "ps2_keyboard.h"
#include "ps2.h"
#include "utils.h"

#define PS2_KEYBOARD_COMMAND_SET_LEDS           0xED
#define PS2_KEYBOARD_COMMAND_SET_SCANCODE       0xF0
#define PS2_KEYBOARD_COMMAND_ENABLE_SCANNING    0xF4
#define PS2_KEYBOARD_COMMAND_DISABLE_SCANNING   0xF5

key_t ps2_keymap_normal[0xFF];
key_t ps2_keymap_shift[0xFF];
key_t ps2_keymap_extended[0xFF];

static void ps2_keyboard_new_byte(ps2_device_t*);

void ps2_init_keyboard(ps2_device_t* keyboard) {
    keyboard->type = PS2_TYPE_KEYBOARD;
    keyboard->callback = ps2_keyboard_new_byte;
    keyboard->keyboard_info.modifiers = 0;

    /* Turn off all keyboard leds (caps lock, num lock, ...)*/
    ps2_device_append_command_queue_with_data(keyboard, PS2_KEYBOARD_COMMAND_SET_LEDS, 0x00, 0);

    /* Set keyboard to use scancode set 2 */
    ps2_device_append_command_queue_with_data(keyboard, PS2_KEYBOARD_COMMAND_SET_SCANCODE, 2, 0);

    /* Enable scanning (reading user keypresses) */
    ps2_device_append_command_queue(keyboard, PS2_KEYBOARD_COMMAND_ENABLE_SCANNING, 0);
}

static void ps2_keyboard_new_byte(ps2_device_t* keyboard) {
    /* If last byte is 0xE0 or 0xF0, key event is not yet ready */
    u8 last_byte = keyboard->byte_buffer[keyboard->byte_buffer_len - 1];
    if (last_byte == 0xE0 || last_byte == 0xF0) {
        return;
    }

    if (last_byte == 0x00 || last_byte == 0xFF) {
        dprintln("PS/2 Keyboard: keydetection error or internal buffer overflow");
        keyboard->byte_buffer_len = 0;
        return;
    }

    if (keyboard->byte_buffer_len > 3) {
        dprintln("PS/2 Keyboard: corrupted package");
        keyboard->byte_buffer_len = 0;
        return;
    }

    u8 index = 0;

    bool extended = false;
    if (index < keyboard->byte_buffer_len && keyboard->byte_buffer[index] == 0xE0) {
        extended = true;
        index++;
    }

    bool released = false;
    if (index < keyboard->byte_buffer_len && keyboard->byte_buffer[index] == 0xF0) {
        released = true;
        index++;
    }

    bool corrupted = (index + 1 != keyboard->byte_buffer_len);
    keyboard->byte_buffer_len = 0;

    if (corrupted)
    {
        dprintln("PS/2 Keyboard: corrupted packet");
        return;
    }

    u8 scancode = keyboard->byte_buffer[index];

    if (keyboard->event_queue_len >= PS2_EVENT_QUEUE_SIZE) {
#if PS2_DUMP_FULL_EVENT_QUEUE
        dprintln("PS/2 Keyboard: event queue full, dropping oldest event");
#endif
        keyboard->event_queue_tail = (keyboard->event_queue_tail + 1) % PS2_EVENT_QUEUE_SIZE;
        keyboard->event_queue_len--;
    }

    key_event_t* event = &keyboard->keyboard_event_queue[keyboard->event_queue_head];

    event->modifiers = keyboard->keyboard_info.modifiers;
    if (released) {
        event->modifiers |= KEY_EVENT_MODIFIERS_RELEASED;
    }

    if (extended) {
        event->key = ps2_keymap_extended[scancode];
    } else if (event->modifiers & KEY_EVENT_MODIFIERS_SHIFT) {
        event->key = ps2_keymap_shift[scancode];
    } else {
        event->key = ps2_keymap_normal[scancode];
    }

    switch (event->key)
    {
        case KEY_LeftShift:
        case KEY_RightShift:
            if (released) {
                keyboard->keyboard_info.modifiers &= ~KEY_EVENT_MODIFIERS_SHIFT;
            } else {
                keyboard->keyboard_info.modifiers |= KEY_EVENT_MODIFIERS_SHIFT;
            }
            break;
        case KEY_LeftCtrl:
        case KEY_RightCtrl:
            if (released) {
                keyboard->keyboard_info.modifiers &= ~KEY_EVENT_MODIFIERS_CONTROL;
            } else {
                keyboard->keyboard_info.modifiers |= KEY_EVENT_MODIFIERS_CONTROL;
            }
            break;
        case KEY_CapsLock:
            if (!released) {
                keyboard->keyboard_info.modifiers ^= KEY_EVENT_MODIFIERS_CAPS_LOCK;
            }
            break;
        default:
            break;
    }

    keyboard->event_queue_head = (keyboard->event_queue_head + 1) % PS2_EVENT_QUEUE_SIZE;
    keyboard->event_queue_len++;
}

void ps2_get_key_event(key_event_t* out) {
    for (u8 i = 0; i < 2; i++) {
        ps2_device_t* device = ps2_get_device(i);
        if (device->type != PS2_TYPE_KEYBOARD) {
            continue;
        }

        ENTER_CRITICAL();
        if (device->event_queue_len == 0) {
            LEAVE_CRITICAL();
            continue;
        }

        memcpy(out, &device->keyboard_event_queue[device->event_queue_tail], sizeof(key_event_t));
        device->event_queue_tail = (device->event_queue_tail + 1) % PS2_EVENT_QUEUE_SIZE;
        device->event_queue_len--;
        LEAVE_CRITICAL();

        return;
    }

    out->key = KEY_NONE;
}
