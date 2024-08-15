#include "critical.h"
#include "debug.h"
#include "ps2_mouse.h"
#include "ps2.h"
#include "utils.h"

#define PS2_MOUSE_COMMAND_SET_SAMPLE_RATE 0xF3

static void ps2_mouse_new_byte(ps2_device_t*);

void ps2_init_mouse(ps2_device_t* mouse) {
    mouse->type = PS2_TYPE_MOUSE;
    mouse->callback = ps2_mouse_new_byte;
    mouse->mouse_info.enabled = false;
    mouse->mouse_info.id = 0xFF;
    mouse->mouse_info.button_mask = 0;

    /* Send magic sequence to query mouse extensions */
    ps2_device_append_command_queue_with_data(mouse, PS2_MOUSE_COMMAND_SET_SAMPLE_RATE, 200, 0);
    ps2_device_append_command_queue_with_data(mouse, PS2_MOUSE_COMMAND_SET_SAMPLE_RATE, 100, 0);
    ps2_device_append_command_queue_with_data(mouse, PS2_MOUSE_COMMAND_SET_SAMPLE_RATE, 80,  0);
    ps2_device_append_command_queue(mouse, PS2_DEVICE_COMMAND_IDENTIFY, 1);
}

static void ps2_mouse_initialize_extensions(ps2_device_t* mouse) {
    u8 last_byte = mouse->byte_buffer[mouse->byte_buffer_len - 1];
    mouse->byte_buffer_len = 0;

    switch (last_byte)
    {
        case 0x00:
            /* No extensions available */
            mouse->mouse_info.id = 0x00;
            mouse->mouse_info.enabled = true;
            break;
        case 0x03:
            if (mouse->mouse_info.id == 0x03) {
                /* No 5 button extension available */
                mouse->mouse_info.enabled = true;
            } else {
                /* Scoll extension available, query for 5 button extension */
                mouse->mouse_info.id = 0x03;
                ps2_device_append_command_queue_with_data(mouse, PS2_MOUSE_COMMAND_SET_SAMPLE_RATE, 200, 0);
                ps2_device_append_command_queue_with_data(mouse, PS2_MOUSE_COMMAND_SET_SAMPLE_RATE, 200, 0);
                ps2_device_append_command_queue_with_data(mouse, PS2_MOUSE_COMMAND_SET_SAMPLE_RATE, 80,  0);
                ps2_device_append_command_queue(mouse, PS2_DEVICE_COMMAND_IDENTIFY, 1);
            }
            break;
        case 0x04:
            /* Scroll and 5 button extensions available! */
            mouse->mouse_info.id = 0x04;
            mouse->mouse_info.enabled = true;
            break;
        default:
            dprint("unknown mouse identification byte ");
            dprintbyte(last_byte);
            dprintln(", assuming no extensions");
            mouse->mouse_info.id = 0x00;
            mouse->mouse_info.enabled = true;
            break;
    }

    /* Set appropriate sample rate and enable mouse scanning */
    if (mouse->mouse_info.enabled) {
        ps2_device_append_command_queue_with_data(mouse, PS2_MOUSE_COMMAND_SET_SAMPLE_RATE, 100, 0);
        ps2_device_append_command_queue(mouse, PS2_DEVICE_COMMAND_ENABLE_SCANNING, 0);
    }
}

static void ps2_mouse_new_byte(ps2_device_t* mouse) {
    /* If mouse is not yet enabled, it means that we are initializing extensions */
    if (!mouse->mouse_info.enabled) {
        ps2_mouse_initialize_extensions(mouse);
        return;
    }

    if (!(mouse->byte_buffer[0] & 0x08)) {
        dprintln("corrupted mouse packet");
        mouse->byte_buffer_len = 0;
        return;
    }

    /* Mouse extensions send 1 extra byte of data */
    int packet_size = (mouse->mouse_info.id == 0x00) ? 3 : 4;

    /* If event packet is not yet sent fully, wait for rest of it */
    if (mouse->byte_buffer_len != packet_size) {
        return;
    }
    mouse->byte_buffer_len = 0;

    /* Ignore packets with bits 6 or 7 set. Qemu sends weird non-standard packets on touchpad when scrolling horizontally. */
    if (mouse->mouse_info.id == 0x04 && (mouse->byte_buffer[3] & 0xC0)) {
        return;
    }

    /* Bits 0-2 define pressed standard buttons */
    u8 button_mask = mouse->byte_buffer[0] & 0x07;

    /* Calculate mouse movement with sign extensions */
    s32 rel_x = mouse->byte_buffer[1] - (((u16)mouse->byte_buffer[0] << 4) & 0x100);
    s32 rel_y = mouse->byte_buffer[2] - (((u16)mouse->byte_buffer[0] << 3) & 0x100);

    /* Mouse IDs 0x03 and 0x04 have byte scroll events */
    s32 scroll = 0;
    if (mouse->mouse_info.id == 0x03 || mouse->mouse_info.id == 0x04) {
        scroll = (mouse->byte_buffer[3] & 0x0F) - ((mouse->byte_buffer[3] << 1) & 0x10);
    }

    /* Mouse ID 0x04 has 2 extra bits for extra buttons */
    if (mouse->mouse_info.id == 0x04) {
        button_mask |= (mouse->byte_buffer[3] >> 1) & 0x18;
    }

    /* There can be at most 7 events per interrupt (5 buttons, move, scroll) */
    mouse_event_t new_events[7];
    int new_event_count = 0;

    /* Check if button mask has changed (buttons pressed or released) */
    if (button_mask != mouse->mouse_info.button_mask) {
        for (int i = 0; i < 5; i++) {
            if ((button_mask & (1 << i)) == (mouse->mouse_info.button_mask & (1 << i))) {
                continue;
            }

            mouse_event_t* event = &new_events[new_event_count++];
            event->type = MOUSE_BUTTON_EVENT;
            event->button_event.pressed = !!(button_mask & (1 << i));
            event->button_event.button = i;
        }

        mouse->mouse_info.button_mask = button_mask;
    }

    /* Check if mouse has moved */
    if (rel_x != 0 || rel_y != 0) {
        mouse_event_t* event = &new_events[new_event_count++];
        event->type = MOUSE_MOVE_EVENT;
        event->move_event.rel_x = rel_x;
        event->move_event.rel_y = rel_y;
    }

    /* Check if mouse has scrolled */
    if (scroll) {
        mouse_event_t* event = &new_events[new_event_count++];
        event->type = MOUSE_SCROLL_EVENT;
        event->scroll_event.value = scroll;
    }

    if (mouse->event_queue_len + new_event_count >= PS2_EVENT_QUEUE_SIZE) {
#if PS2_DUMP_FULL_EVENT_QUEUE
        dprintln("PS/2 Mouse: event queue full, dropping oldest event");
#endif
        mouse->event_queue_tail = (mouse->event_queue_tail + new_event_count) % PS2_EVENT_QUEUE_SIZE;
        mouse->event_queue_len -= new_event_count;
    }

    /* Add events to mouse event queue */
    for (int i = 0; i < new_event_count; i++) {
        memcpy(&mouse->mouse_event_queue[mouse->event_queue_head], &new_events[i], sizeof(mouse_event_t));
        mouse->event_queue_head = (mouse->event_queue_head + 1) % PS2_EVENT_QUEUE_SIZE;
        mouse->event_queue_len++;
    }
}

void ps2_get_mouse_event(mouse_event_t* out) {
    for (u8 i = 0; i < 2; i++) {
        ps2_device_t* device = ps2_get_device(i);
        if (device->type != PS2_TYPE_MOUSE) {
            continue;
        }

        ENTER_CRITICAL();
        if (device->event_queue_len == 0) {
            LEAVE_CRITICAL();
            continue;
        }

        memcpy(out, &device->mouse_event_queue[device->event_queue_tail], sizeof(mouse_event_t));
        device->event_queue_tail = (device->event_queue_tail + 1) % PS2_EVENT_QUEUE_SIZE;
        device->event_queue_len--;
        LEAVE_CRITICAL();

        return;
    }

    out->type = MOUSE_EVENT_NONE;
}
