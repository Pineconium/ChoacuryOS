#include "ps2_keyboard.h"
#include "ps2.h"
#include "utils.h"
#include "vga.h"

#define PS2_KEYBOARD_COMMAND_SET_LEDS           0xED
#define PS2_KEYBOARD_COMMAND_SET_SCANCODE       0xF0
#define PS2_KEYBOARD_COMMAND_ENABLE_SCANNING    0xF4
#define PS2_KEYBOARD_COMMAND_DISABLE_SCANNING   0xF5

key_t ps2_keymap_normal[0xFF];
key_t ps2_keymap_shift[0xFF];
key_t ps2_keymap_extended[0xFF];

void ps2_init_keyboard(ps2_device_t* keyboard) {
    keyboard->type = PS2_TYPE_KEYBOARD;
	keyboard->keyboard_info.modifiers = 0;

    /* Turn off all keyboard leds (caps lock, num lock, ...)*/
	ps2_device_append_command_queue(keyboard, PS2_KEYBOARD_COMMAND_SET_LEDS);
	ps2_device_append_command_queue(keyboard, 0x00);

    /* Set keyboard to use scancode set 2 */
    ps2_device_append_command_queue(keyboard, PS2_KEYBOARD_COMMAND_SET_SCANCODE);
    ps2_device_append_command_queue(keyboard, 2);

    /* Enable scanning (reading user keypresses) */
    ps2_device_append_command_queue(keyboard, PS2_KEYBOARD_COMMAND_ENABLE_SCANNING);
}

void ps2_keyboard_new_byte(ps2_device_t* keyboard) {
	/* If last byte is 0xE0 or 0xF0, key event is not yet ready */
	u8 last_byte = keyboard->byte_buffer[keyboard->byte_buffer_len - 1];
	if (last_byte == 0xE0 || last_byte == 0xF0) {
		return;
	}

	u32 scancode = 0;
	bool extended = false;
	bool released = false;

	/* NOTE: This assumes that keyboard sends valid data */
	for (u8 i = 0; i < keyboard->byte_buffer_len; i++) {
		if (keyboard->byte_buffer[i] == 0xE0) {
			extended = true;
		} else if (keyboard->byte_buffer[i] == 0xF0) {
			released = true;
		} else {
			scancode = (scancode << 8) | keyboard->byte_buffer[i];
		}
	}

	if (keyboard->event_queue_len >= PS2_EVENT_QUEUE_SIZE) {
		k_printf("PS/2 Keyboard: event queue full", 0, TC_YELLO);
		keyboard->event_queue_len %= PS2_EVENT_QUEUE_SIZE;
	}

	key_event_t* event = &keyboard->keyboard_event_queue[keyboard->event_queue_len].key_event;

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
	}

	keyboard->event_queue_len++;
	keyboard->byte_buffer_len = 0;
}

void ps2_get_key_event(key_event_t* out) {
	for (u8 i = 0; i < 2; i++) {
		ps2_device_t* device = ps2_get_device(i);
		if (device->type != PS2_TYPE_KEYBOARD) {
			continue;
		}
		if (device->event_queue_len == 0) {
			continue;
		}
		
		key_event_t* event = &device->keyboard_event_queue[0].key_event;
		out->key		= event->key;
		out->modifiers	= event->modifiers;

		/* Shift other events */
		device->event_queue_len--;
		memory_move(
			(u8*)&device->keyboard_event_queue[0],
			(u8*)&device->keyboard_event_queue[1],
			device->event_queue_len * sizeof(key_event_t)
		);

		return;
	}

	out->key = KEY_NONE;
}
