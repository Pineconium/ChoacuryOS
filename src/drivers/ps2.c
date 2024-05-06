#include "critical.h"
#include "debug.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include "ports.h"
#include "ps2.h"

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

#define PS2_STATE_NORMAL	0
#define PS2_STATE_WAIT_ACK	1
#define PS2_STATE_WAIT_RESP	2

static const u64 s_timeout_ms = 100;

static ps2_device_t s_devices[2];

typedef struct {
    ps2_device_t* device;
    u8 data[2];
    u8 data_size;
    u8 data_sent;
    u8 resp_size;
    u8 state;
} ps2_command_t;

static ps2_command_t s_command_queue[PS2_COMMAND_QUEUE_SIZE];
static u8 s_command_queue_read  = 0;
static u8 s_command_queue_write = 0;

static int ps2_send_byte(u16 port, u8 data) {
    u64 timeout = pit_current_time_ms() + s_timeout_ms;
    while (pit_current_time_ms() < timeout) {
        if (port_byte_in(PS2_PORT_STATUS) & PS2_STATUS_INPUT) {
            continue;
        }
        port_byte_out(port, data);
        return 0;
    }
    return -1;
}

static int ps2_read_byte() {
    u64 timeout = pit_current_time_ms() + s_timeout_ms;
    while (pit_current_time_ms() < timeout) {
        if (!(port_byte_in(PS2_PORT_STATUS) & PS2_STATUS_OUTPUT)) {
            continue;
        }
        return port_byte_in(PS2_PORT_DATA);
    }
    return -1;
}

static int ps2_controller_send_command(u8 command) {
    if (ps2_send_byte(PS2_PORT_COMMAND, command) == -1) {
        return -1;
    }
    return 0;
}

static int ps2_controller_send_command_with_data(u8 command, u8 data) {
    if (ps2_send_byte(PS2_PORT_COMMAND, command) == -1) {
        return -1;
    }
    if (ps2_send_byte(PS2_PORT_DATA, data) == -1) {
        return -1;
    }
    return 0;
}

static int ps2_device_send_byte(ps2_device_t* device, u8 byte) {
    if (device == &s_devices[1]) {
        if (ps2_send_byte(PS2_PORT_COMMAND, PS2_COMMAND_WRITE_TO_SECOND) == -1) {
            return -1;
        }
    }
    if (ps2_send_byte(PS2_PORT_DATA, byte) == -1) {
        return -1;
    }
    return 0;
}

static int ps2_device_send_byte_and_wait_ack(ps2_device_t* device, u8 byte) {
    for (;;) {
        int resp = ps2_device_send_byte(device, byte);
        if (resp == -1) {
            return -1;
        }

        resp = ps2_read_byte();
        if (resp == -1)
            return -1;
        if (resp == PS2_DEVICE_RESEND)
            continue;
        if (resp == PS2_DEVICE_ACK)
            break;
        dprintln("PS/2 Controller: expected ACK");
    }
    return 0;
}

static void ps2_update_command_queue() {
    ENTER_CRITICAL();
    if (s_command_queue_read == s_command_queue_write) {
        LEAVE_CRITICAL();
        return;
    }
    ps2_command_t* command = &s_command_queue[s_command_queue_read];
    LEAVE_CRITICAL();

    if (command->state != PS2_STATE_NORMAL) {
        return;
    }

    command->state = PS2_STATE_WAIT_ACK;
    if (ps2_device_send_byte(command->device, command->data[command->data_sent]) == -1) {
        dprint("PS/2 Controller: command send timeout");
        command->state = PS2_STATE_NORMAL;
    }
}

void ps2_device_append_command_queue(ps2_device_t* device, u8 byte, u8 response_size) {
    ENTER_CRITICAL();
    if ((s_command_queue_write + 1) % PS2_COMMAND_QUEUE_SIZE == s_command_queue_read) {
        dprintln("PS/2 Controller: command queue full, ignoring command");
        LEAVE_CRITICAL();
        return;
    }
    ps2_command_t* command = &s_command_queue[s_command_queue_write];
    s_command_queue_write = (s_command_queue_write + 1) % PS2_COMMAND_QUEUE_SIZE;
    LEAVE_CRITICAL();

    command->device = device;
    command->data[0] = byte;
    command->data[1] = 0x00;
    command->data_size = 1;
    command->data_sent = 0;
    command->resp_size = response_size;
    command->state = PS2_STATE_NORMAL;
}

void ps2_device_append_command_queue_with_data(ps2_device_t* device, u8 byte0, u8 byte1, u8 response_size) {
    ENTER_CRITICAL();
    if ((s_command_queue_write + 1) % PS2_COMMAND_QUEUE_SIZE == s_command_queue_read) {
        dprintln("PS/2 Controller: command queue full, ignoring command");
        LEAVE_CRITICAL();
        return;
    }
    ps2_command_t* command = &s_command_queue[s_command_queue_write];
    s_command_queue_write = (s_command_queue_write + 1) % PS2_COMMAND_QUEUE_SIZE;
    LEAVE_CRITICAL();

    command->device = device;
    command->data[0] = byte0;
    command->data[1] = byte1;
    command->data_size = 2;
    command->data_sent = 0;
    command->resp_size = response_size;
    command->state = PS2_STATE_NORMAL;
}

static void ps2_irq_handler(ps2_device_t* device) {
    u8 byte = port_byte_in(PS2_PORT_DATA);

    bool handle_command = true;
    if (s_command_queue_read == s_command_queue_write)
        handle_command = false;
    else if (s_command_queue[s_command_queue_read].device != device)
        handle_command = false;
    else if (s_command_queue[s_command_queue_read].state == PS2_STATE_NORMAL)
        handle_command = false;

    bool send_to_device = !handle_command;

    if (handle_command) {
        ps2_command_t* command = &s_command_queue[s_command_queue_read];
        if (command->state == PS2_STATE_WAIT_ACK) {
            if (byte == PS2_DEVICE_ACK) {
                command->data_sent++;
                if (command->data_sent < command->data_size) {
                    command->state = PS2_STATE_NORMAL;
                } else if (command->resp_size > 0) {
                    command->state = PS2_STATE_WAIT_RESP;
                } else {
                    s_command_queue_read++;
                }
            } else if (byte == PS2_DEVICE_RESEND) {
                command->state = PS2_STATE_NORMAL;
            }
        } else if (command->state == PS2_STATE_WAIT_RESP) {
            if (--command->resp_size <= 0) {
                s_command_queue_read++;
                send_to_device = true;
            }
        }
    }

    if (send_to_device) {
        device->byte_buffer[device->byte_buffer_len++] = byte;
        switch (device->type) {
            case PS2_TYPE_KEYBOARD:
                ps2_keyboard_new_byte(device);
                break;
            default:
                dprintln("PS/2 Controller: interrupt from uninitialized device");
                break;
        }
    }

    ps2_update_command_queue();
}

static void ps2_irq1_handler() {
    ps2_irq_handler(&s_devices[0]);
}

static void ps2_irq2_handler() {
    ps2_irq_handler(&s_devices[1]);
}

static void ps2_zero_device(ps2_device_t* device) {
    device->type = PS2_TYPE_NONE;
    device->byte_buffer_len = 0;
    device->event_queue_len = 0;
}

ps2_device_t* ps2_get_device(u8 index) {
    return &s_devices[index];
}

#define TRY(expr) ({ int ret = expr; if (ret == -1) { dprintln("PS/2 Controller: timeout"); return; }; ret; })

void ps2_init() {
    ps2_zero_device(&s_devices[0]);
    ps2_zero_device(&s_devices[1]);

    /* FIXME: Parse ACPI tables to confirm that PS/2 actually exists */

    /* Disable devices */
    TRY(ps2_controller_send_command(PS2_COMMAND_DISABLE_FIRST));
    TRY(ps2_controller_send_command(PS2_COMMAND_DISABLE_SECOND));

    /* Flush data in controller */
    while (ps2_read_byte() != -1) {
        continue;
    }

    /* Read current config and turn off translation and interrupts */
    TRY(ps2_controller_send_command(PS2_COMMAND_READ_CONFIG));
    u8 config = ps2_read_byte();
    config &= ~PS2_CONFIG_INTERRUPT_FIRST;
    config &= ~PS2_CONFIG_INTERRUPT_SECOND;
    config &= ~PS2_CONFIG_TRANSLATION_FIRST;
    TRY(ps2_controller_send_command_with_data(PS2_COMMAND_WRITE_CONFIG, config));

    /* Perform self test */
    TRY(ps2_controller_send_command(PS2_COMMAND_TEST_CONTROLLER));
    if (TRY(ps2_read_byte()) != 0x55) {
        dprintln("PS2 Controller: self test failed, abort intialiation");
        return;
    }
    /* NOTE: controller might reset config, so resend it */
    TRY(ps2_controller_send_command_with_data(PS2_COMMAND_WRITE_CONFIG, config));

    bool valid_ports[] = { true, false };
    if (config & PS2_CONFIG_CLOCK_SECOND) {
        ps2_controller_send_command(PS2_COMMAND_ENABLE_SECOND);
        ps2_controller_send_command(PS2_COMMAND_READ_CONFIG);
        if (!(ps2_read_byte() & PS2_CONFIG_CLOCK_SECOND)) {
            ps2_controller_send_command(PS2_COMMAND_DISABLE_SECOND);
            valid_ports[1] = true;
        }
    }

    /* Perform interface tests */
    for (int i = 0; i < 2; i++) {
        if (!valid_ports[i]) {
            continue;
        }

        TRY(ps2_controller_send_command(i == 0 ? PS2_COMMAND_TEST_FIRST : PS2_COMMAND_TEST_SECOND));
        if (TRY(ps2_read_byte()) != PS2_DEVICE_TEST_PASS) {
            dprintln("PS2 Controller: port self test failed");
            valid_ports[i] = false;
        }
    }
    if (!valid_ports[0] && !valid_ports[1]) {
        dprintln("PS2 Controller: no devices");
        return;
    }

    /* Enable and reset devices */
    for (int i = 0; i < 2; i++) {
        if (!valid_ports[i]) {
            continue;
        }

        TRY(ps2_controller_send_command(i == 0 ? PS2_COMMAND_ENABLE_FIRST : PS2_COMMAND_ENABLE_SECOND));

        TRY(ps2_device_send_byte_and_wait_ack(&s_devices[i], PS2_DEVICE_COMMAND_RESET));
        while (ps2_read_byte() != -1) {
            continue;
        }

        TRY(ps2_device_send_byte_and_wait_ack(&s_devices[i], PS2_DEVICE_COMMAND_DISABLE_SCANNING));

        TRY(ps2_device_send_byte_and_wait_ack(&s_devices[i], PS2_DEVICE_COMMAND_IDENTIFY));

        u8 response[2];
        u8 response_size = 0;

        dprint("PS2 Controller: ");
        for (int j = 0; j < 2; j++) {
            int data = ps2_read_byte();
            if (data == -1) {
                break;
            }
            response[response_size++] = data;

            dprintbyte(data);
            dprint(" ");
        }

        if (response_size == 2 && response[0] == 0xAB && response[1] == 0x83) {
            ps2_init_keyboard(&s_devices[i]);
            dprintln("=> keyboard");
        } else {
            dprintln("=> unsupported device");
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

    TRY(ps2_controller_send_command_with_data(PS2_COMMAND_WRITE_CONFIG, config));
    ps2_update_command_queue();
}
