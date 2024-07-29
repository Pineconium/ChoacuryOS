/* actually moose drivers yay */

#include "ports.h"
#include "types.h"
#include "pic.h"
#include "mouse.h"
#include "pointerctrl.h"    // <-- where 'Point' is
#include "../gui/desktop.h"


Point MousePosition;
Point MousePositionOld;

/* process the mouse packet */
void process_mouse_pack(){
    if (PacketReady == 0) return;

        int xNegative, yNegative, xOverflow, yOverflow;

        if (MousePacket[0] & PS2XSign){
            xNegative = 1;
        }else xNegative = 0;

        if (MousePacket[0] & PS2YSign){
            yNegative = 1;
        }else yNegative = 0;

        if (MousePacket[0] & PS2XOverflow){
            xOverflow = 1;
        }else xOverflow = 0;

        if (MousePacket[0] & PS2YOverflow){
            yOverflow = 1;
        }else yOverflow = 0;

        if (!xNegative){
            MousePosition.X += MousePacket[1];
            if (xOverflow == 1){
                MousePosition.X += 255;
            }
        } else
        {
            MousePacket[1] = 256 - MousePacket[1];
            MousePosition.X -= MousePacket[1];
            if (xOverflow == 1){
                MousePosition.X -= 255;
            }
        }

        if (!yNegative){
            MousePosition.Y -= MousePacket[2];
            if (yOverflow == 1){
                MousePosition.Y -= 255;
            }
        } else
        {
            MousePacket[2] = 256 - MousePacket[2];
            MousePosition.Y += MousePacket[2];
            if (yOverflow == 1){
                MousePosition.Y += 255;
            }
        }

        if (MousePosition.X < 0) MousePosition.X = 0;
        if (MousePosition.X > 320-1) MousePosition.X = 320-1;
        
        if (MousePosition.Y < 0) MousePosition.Y = 0;
        if (MousePosition.Y > 200-1) MousePosition.Y = 200-1;
        
        clear_pointer(Cursor, MousePositionOld);
        draw_pointer(Cursor, MousePosition, 1);

        if (MousePacket[0] & PS2Leftbutton){
        }
        if (MousePacket[0] & PS2Middlebutton){        
        }
        if (MousePacket[0] & PS2Rightbutton){
        }

        PacketReady = 0;
        MousePositionOld = MousePosition;
}

/* mouse functions */
void mouse_init(){
    port_byte_out(0x64, 0xA8);

    mouse_wait();
    port_byte_out(0x64, 0x20);
    mouse_wait_input();
    uint8_t status = port_byte_in(0x60);
    status |= 0b10;
    mouse_wait();
    port_byte_out(0x64, 0x60);
    mouse_wait();
    port_byte_out(0x60, status);

    mouse_write(0xF6);
    mouse_read();

    mouse_write(0xF4);
    mouse_read();
}

void mouse_write(uint8_t value){
    mouse_wait();
    port_byte_out(0x64, 0xD4);
    mouse_wait();
    port_byte_out(0x60, value);
}

uint8_t mouse_read(){
    mouse_wait_input();
    return port_byte_in(0x60);
}

void mouse_wait(){
    uint64_t timeout = 100000;
    while (timeout--){
        if ((port_byte_in(0x64) & 0b10) == 0){
            return;
        }
    }
}

void mouse_wait_input(){
    uint64_t timeout = 100000;
    while (timeout--){
        if (port_byte_in(0x64) & 0b1){
            return;
        }
    }
}

void handle_mouse(uint8_t data){
    process_mouse_pack();
    static int skip = 1;
    if (skip == 1) { skip = 0; return; }
    
    switch(Cycle){
        case 0:
            if (PacketReady == 1) break;
            if ((data & 0b00001000) == 0) break;
            MousePacket[0] = data;
            Cycle++;
            break;
        case 1:
            if (PacketReady == 1) break;
            MousePacket[1] = data;
            Cycle++;
            break;
        case 2:
            if (PacketReady == 1) break;
            MousePacket[2] = data;
            PacketReady = 1;
            Cycle = 0;
            break;
    }
}