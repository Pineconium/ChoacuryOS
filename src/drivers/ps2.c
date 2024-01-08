#include "idt.h"
#include "pic.h"
#include "ports.h"
#include "ps2.h"
#include "vga.h"

#define PS2_PORT_DATA       0x60
#define PS2_PORT_STATUS     0x64
#define PS2_PORT_COMMAND    0x64

#define PS2_COMMAND_READ_CONFIG     0x20
#define PS2_COMMAND_WRITE_CONFIG    0x60
#define PS2_COMMAND_DISABLE_SECOND  0xA7
#define PS2_COMMAND_ENABLE_SECOND   0xA8
#define PS2_COMMAND_TEST_SECOND     0xA9
#define PS2_COMMAND_TEST_CONTROLLER 0xAA
#define PS2_COMMAND_TEST_FIRST      0xAB
#define PS2_COMMAND_DISABLE_FIRST   0xAD
#define PS2_COMMAND_ENABLE_FIRST    0xAE
#define PS2_COMMAND_WRITE_TO_SECOND 0xD4

#define PS2_DEVICE_COMMAND_IDENTIFY         0xF2
#define PS2_DEVICE_COMMAND_ENABLE_SCANNING  0xF4
#define PS2_DEVICE_COMMAND_DISABLE_SCANNING 0xF5
#define PS2_DEVICE_COMMAND_RESET            0xFF

#define PS2_DEVICE_TEST_PASS    0x00
#define PS2_DEVICE_ACK          0xFA
#define PS2_DEVICE_RESEND       0xFE

#define PS2_STATUS_OUTPUT   (1 << 0)
#define PS2_STATUS_INPUT    (1 << 1)

#define PS2_CONFIG_INTERRUPT_FIRST      (1 << 0)
#define PS2_CONFIG_INTERRUPT_SECOND     (1 << 1)
#define PS2_CONFIG_CLOCK_SECOND         (1 << 5)
#define PS2_CONFIG_TRANSLATION_FIRST    (1 << 6)

#define PS2_INTERRUPT_FIRST     1
#define PS2_INTERRUPT_SECOND    12

static ps2_device_t s_devices[2];

static void ps2_controller_send_command(u8 command) {
    port_byte_out(PS2_PORT_COMMAND, command);
}

static void ps2_controller_send_command_with_data(u8 command, u8 data) {
    port_byte_out(PS2_PORT_COMMAND, command);
    while (port_byte_in(PS2_PORT_STATUS) & PS2_STATUS_INPUT) {
        continue;
    }
    port_byte_out(PS2_PORT_DATA, data);
}

static u8 ps2_controller_read_byte() {
    while (!(port_byte_in(PS2_PORT_STATUS) & PS2_STATUS_OUTPUT)) {
        continue;
    }
    return port_byte_in(PS2_PORT_DATA);
}

void ps2_device_send_byte(ps2_device_t* device, u8 byte) {
    if (device == &s_devices[1]) {
        ps2_controller_send_command(PS2_COMMAND_WRITE_TO_SECOND);
    }
    /* FIXME: timeout */
    while (port_byte_in(PS2_PORT_STATUS) & PS2_STATUS_INPUT) {
        continue;
    }
    port_byte_out(PS2_PORT_DATA, byte);
}

static u8 ps2_device_read_byte(ps2_device_t*) {
    /* FIXME: timeout */
    while (!(port_byte_in(PS2_PORT_STATUS) & PS2_STATUS_OUTPUT)) {
        continue;
    }
    return port_byte_in(PS2_PORT_DATA);
}

static void ps2_device_wait_ack(ps2_device_t* device) {
    /* FIXME: timeout */
    while (ps2_device_read_byte(device) != PS2_DEVICE_ACK) {
        continue;
    }
}

static void ps2_update_device(ps2_device_t* device) {
    if (device->type == PS2_TYPE_NONE) {
        return;
    }
    if (device->command_queue_read == device->command_queue_write) {
        return;
    }
    device->state = PS2_STATE_WAIT_ACK;
    ps2_device_send_byte(device, device->command_queue[device->command_queue_read]);
}

void ps2_device_append_command_queue(ps2_device_t* device, u8 byte) {
    device->command_queue[device->command_queue_write] = byte;
    device->command_queue_write = (device->command_queue_write + 1) % PS2_COMMAND_QUEUE_SIZE;
}

static void ps2_irq_handler(ps2_device_t* device) {
    u8 byte = ps2_device_read_byte(device);

    if (device->state == PS2_STATE_WAIT_ACK) {
        switch (byte) {
			case PS2_DEVICE_ACK:
				device->command_queue_read = (device->command_queue_read + 1) % PS2_COMMAND_QUEUE_SIZE;
				device->state = PS2_STATE_NORMAL;
				break;
			case PS2_DEVICE_RESEND:
				device->state = PS2_STATE_NORMAL;
				break;
			default:
				k_printf("PS/2 device: unhandeled byte", 0, TC_LRED);
				break;
		}
    } else {
        device->byte_buffer[device->byte_buffer_len++] = byte;
        switch (device->type) {
            case PS2_TYPE_KEYBOARD:
                ps2_keyboard_new_byte(device);
                break;
            default:
                k_printf("Interrupt from uninitialized device", 0, TC_LRED);
                break;
        }
    }

    if (device->state == PS2_STATE_NORMAL) {
        ps2_update_device(device);
    }
}

static void ps2_irq1_handler() {
    ps2_irq_handler(&s_devices[0]);
}

static void ps2_irq2_handler() {
    ps2_irq_handler(&s_devices[1]);
}

static void ps2_zero_device(ps2_device_t* device) {
    device->type = PS2_TYPE_NONE;
	device->state = PS2_STATE_NORMAL;
    device->command_queue_read = 0;
    device->command_queue_write = 0;
    device->byte_buffer_len = 0;
    device->event_queue_len = 0;
}

ps2_device_t* ps2_get_device(u8 index) {
    return &s_devices[index];
}

void ps2_init() {
    ps2_zero_device(&s_devices[0]);
    ps2_zero_device(&s_devices[1]);

    /* FIXME: Parse ACPI tables to confirm that PS/2 actually exists */

    /* Disable devices */
    ps2_controller_send_command(PS2_COMMAND_DISABLE_FIRST);
    ps2_controller_send_command(PS2_COMMAND_DISABLE_SECOND);

    /* Flush data in controller */
    while (port_byte_in(PS2_PORT_STATUS) & PS2_STATUS_OUTPUT)
        port_byte_in(PS2_PORT_DATA);

    /* Read current config and turn off translation and interrupts */
    ps2_controller_send_command(PS2_COMMAND_READ_CONFIG);
    u8 config = ps2_controller_read_byte();
    config &= PS2_CONFIG_INTERRUPT_FIRST;
    config &= PS2_CONFIG_INTERRUPT_SECOND;
    config &= PS2_CONFIG_TRANSLATION_FIRST;
    ps2_controller_send_command_with_data(PS2_COMMAND_WRITE_CONFIG, config);

    /* Perform self test */
    ps2_controller_send_command(PS2_COMMAND_TEST_CONTROLLER);
    if (ps2_controller_read_byte() != 0x55) {
        k_printf("PS2 controller self test failed, abort intialiation", 0, TC_LRED);
        return;
    }

    /* Determine if we have one or two ports */
    bool valid_ports[] = { true, false };
    if (config & PS2_CONFIG_CLOCK_SECOND) {
        ps2_controller_send_command(PS2_COMMAND_ENABLE_SECOND);
        ps2_controller_send_command(PS2_COMMAND_READ_CONFIG);
        if (ps2_controller_read_byte() & PS2_CONFIG_CLOCK_SECOND) {
            ps2_controller_send_command(PS2_COMMAND_DISABLE_SECOND);
            valid_ports[1] = true;
        }
    }

    /* Perform interface tests */
    for (int i = 0; i < 2; i++) {
        if (valid_ports[i]) {
            ps2_controller_send_command(i == 0 ? PS2_COMMAND_TEST_FIRST : PS2_COMMAND_TEST_SECOND);
            if (ps2_controller_read_byte() != PS2_DEVICE_TEST_PASS) {
                valid_ports[i] = false;
            }
        }
    }
    if (!valid_ports[0] && !valid_ports[1]) {
        k_printf("PS2 controller has no devices", 0, TC_YELLO);
        return;
    }

    /* Enable and reset devices */
    for (int i = 0; i < 2; i++) {
        if (!valid_ports[i]) {
            continue;
        }

        ps2_controller_send_command(i == 0 ? PS2_COMMAND_ENABLE_FIRST : PS2_COMMAND_ENABLE_SECOND);

        ps2_device_send_byte(&s_devices[i], PS2_DEVICE_COMMAND_DISABLE_SCANNING);
        ps2_device_wait_ack(&s_devices[i]);

        ps2_device_send_byte(&s_devices[i], PS2_DEVICE_COMMAND_RESET);
        ps2_device_wait_ack(&s_devices[i]);

        ps2_device_send_byte(&s_devices[i], PS2_DEVICE_COMMAND_DISABLE_SCANNING);
        ps2_device_wait_ack(&s_devices[i]);
    }

    /* Identify devices */
    for (int i = 0; i < 2; i++) {
        if (!valid_ports[i]) {
            continue;
        }
        
        ps2_device_send_byte(&s_devices[i], PS2_DEVICE_COMMAND_IDENTIFY);
        ps2_device_wait_ack(&s_devices[i]);

        u8 response[2];

        /* FIXME: some devices respond with only 1 byte. Timeout will be needed for this */
        for (int j = 0; j < 2; j++) {
            response[j] = ps2_device_read_byte(&s_devices[i]);
        }
        
        if (response[0] == 0xAB && response[1] == 0x83) {
            ps2_init_keyboard(&s_devices[i]);
        }
    }

    /* Enable interrupts on valid ports */
    if (s_devices[0].type != PS2_TYPE_NONE) {
        idt_register_irq_handler(PS2_INTERRUPT_FIRST, ps2_irq1_handler);
        pic_unmask(PS2_INTERRUPT_FIRST);
        config |= PS2_CONFIG_INTERRUPT_FIRST;
    }
    if (s_devices[1].type != PS2_TYPE_NONE) {
        idt_register_irq_handler(PS2_INTERRUPT_SECOND, ps2_irq2_handler);
        pic_unmask(PS2_INTERRUPT_SECOND);
        config |= PS2_CONFIG_INTERRUPT_SECOND;
    }
    
    ps2_controller_send_command_with_data(PS2_COMMAND_WRITE_CONFIG, config);

    ps2_update_device(&s_devices[0]);
    ps2_update_device(&s_devices[1]);
}
