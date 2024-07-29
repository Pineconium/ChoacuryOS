#include "vga.h"
#include "ports.h"
#include "pointerctrl.h"
#include "../gui/desktop.h"
/*
VGA Stuff, aka. (Extremely Limited) Graphics.

Currently this will only work with Virtual Machines for some reason.
*/


//the vga identifiers
unsigned int   VGA_width;
unsigned int   VGA_height;
unsigned int   VGA_bpp;
unsigned char *VGA_address;


uint32_t MouseCursorBuffer[16 * 16];
uint32_t MouseCursorBufferAfter[16 * 16];
int MouseDrawn = false;


void vga_set_char(u32 x, u32 y, u8 ch, u8 color) {
    u16* base = (u16*)0xB8000;
    base[y * VGA_WIDTH + x] = ((u16)color << 8) | ch;
}

void vga_move_cursor(u32 x, u32 y) {
    u16 pos = y * VGA_WIDTH + x;
    port_byte_out(0x3D4, 0x0F);
    port_byte_out(0x3D5, (u8)(pos & 0xFF));
    port_byte_out(0x3D4, 0x0E);
    port_byte_out(0x3D5, (u8)(pos >> 8));
}

/*TODO: Put abs(int) somewhere else */
int abs(int x) {
    return x < 0 ? -x : x;
}


void write_registers(unsigned char *regs){
	unsigned i;

	/* write MISCELLANEOUS reg */
	port_byte_out(VGA_MISC_WRITE, *regs);
	regs++;
	/* write SEQUENCER regs */
	for (i = 0; i < VGA_NUM_SEQ_REGS; i++)
	{
		port_byte_out(VGA_SEQ_INDEX, i);
		port_byte_out(VGA_SEQ_DATA, *regs);
		regs++;
	}
	/* unlock CRTC registers */
	port_byte_out(VGA_CRTC_INDEX, 0x03);
	port_byte_out(VGA_CRTC_DATA, port_byte_in(VGA_CRTC_DATA) | 0x80);
	port_byte_out(VGA_CRTC_INDEX, 0x11);
	port_byte_out(VGA_CRTC_DATA, port_byte_in(VGA_CRTC_DATA) & ~0x80);
	/* make sure they remain unlocked */
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;
	/* write CRTC regs */
	for (i = 0; i < VGA_NUM_CRTC_REGS; i++)
	{
		port_byte_out(VGA_CRTC_INDEX, i);
		port_byte_out(VGA_CRTC_DATA, *regs);
		regs++;
	}
	/* write GRAPHICS CONTROLLER regs */
	for (i = 0; i < VGA_NUM_GC_REGS; i++)
	{
		port_byte_out(VGA_GC_INDEX, i);
		port_byte_out(VGA_GC_DATA, *regs);
		regs++;
	}
	/* write ATTRIBUTE CONTROLLER regs */
	for (i = 0; i < VGA_NUM_AC_REGS; i++)
	{
		(void)port_byte_in(VGA_INSTAT_READ);
		port_byte_out(VGA_AC_INDEX, i);
		port_byte_out(VGA_AC_WRITE, *regs);
		regs++;
	}

	/* lock 16-color palette and unblank display */
	(void)port_byte_in(VGA_INSTAT_READ);
	port_byte_out(VGA_AC_INDEX, 0x20);
}

/**
* Clears the VGA screen
*/
void VGA_clear_screen(unsigned char color)
{
	unsigned int x = 0;
	unsigned int y = 0;

	for (y = 0; y < VGA_height; y++)
	{
		for (x = 0; x < VGA_width; x++)
		{
			VGA_address[VGA_width * y + x] = color;
		}
	}
}

void putpixel(unsigned int x, unsigned int y, unsigned char color) {
    if (x < VGA_width && y < VGA_height) {
        VGA_address[(y * VGA_width) + x] = color;
    }
}

void draw_line(int x1, int y1, int x2, int y2, unsigned char color) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1; 
    int err = dx + dy, e2;

    while (1) {
        putpixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void fillrect(unsigned char sx, unsigned char sy, unsigned char ex, unsigned char ey, unsigned char clr) {
	int w, h;
	for(w = sx; w < ex; w++){
		for(h = sy; h < ey; h++){
			putpixel(w,h,clr);
		}
	}
}

u32 getpixel(u32 x, u32 y){
    return *(u32*)(0xA0000 + (x*4) + (y * 320 /*<-- Dont hardcode this, what if other modes are used and PixelsPerScanLine are <320*/ * 4));
}

// TODO: fix it. & Add fonts back (removed to save space)
/*
unsigned char *font = VGA_fonts;
void drawchar(unsigned char c, int x, int y, int fgcolor, int bgcolor){
	int cx,cy;
	int mask[8]={1,2,4,8,16,32,64,128};
	unsigned char *glyph=font+(int)c*16;

	for(cy=0;cy<16;cy++){
		for(cx=0;cx<8;cx++){
			putpixel(glyph[cy]&mask[cx]?fgcolor:bgcolor,x+cx,y+cy-12);
		}
	}
}
*/

void vga_init(unsigned char clr){
	int width  = 320;
	int height = 200;
	int bpp    = 256;
	/* setup vga struct. */
	VGA_width   = (unsigned int)width;
	VGA_height  = (unsigned int)height;
	VGA_bpp     = bpp;
	VGA_address = (void *)0xA0000;

	/* actually switch to mode 13 */
	write_registers(mode_320_200_256);

	/* clear the screen */
	VGA_clear_screen(clr);
	
	Desktop();
}

void vga_text_init(unsigned char clr)
{
	int width  = 80;
	int height = 25;
	int bpp    = 8;
	//setup the vga struct
	VGA_width   = (unsigned int)width;
	VGA_height  = (unsigned int)height;
	VGA_bpp     = bpp;
	VGA_address = (void *)0xB8000;

	/* actually switch to mode 13 */
	write_registers(mode_80x25_text);

	//clears the screen
	VGA_clear_screen(clr);
	// return to shell?
	
}

/* mouse pointer stuff */

void draw_pointer(uint8_t* Cursor, Point position, unsigned char colour){

    int xMax = 16;
    int yMax = 16;
    int differenceX = 320 - position.X;
    int differenceY = 200 - position.Y;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;

    for (int y = 0; y < yMax; y++){
        for (int x = 0; x < xMax; x++){
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((Cursor[byte] & (0b10000000 >> (x % 8))))
            {
                MouseCursorBuffer[x + y * 16] = getpixel(position.X + x, position.Y + y);
                putpixel(position.X + x, position.Y + y, colour);
                MouseCursorBufferAfter[x + y * 16] = getpixel(position.X + x, position.Y + y);

            }
        }
    }

    MouseDrawn = true;
}

void clear_pointer(uint8_t* Cursor, Point position){
    if (MouseDrawn == 0) return;

    int xMax = 16;
    int yMax = 16;
    int differenceX = 320 - position.X;
    int differenceY = 200 - position.Y;

    if (differenceX < 16) xMax = differenceX;
    if (differenceY < 16) yMax = differenceY;

    for (int y = 0; y < yMax; y++){
        for (int x = 0; x < xMax; x++){
            int bit = y * 16 + x;
            int byte = bit / 8;
            if ((Cursor[byte] & (0b10000000 >> (x % 8))))
            {
                if (getpixel(position.X + x, position.Y + y) == MouseCursorBufferAfter[x + y *16]){
                    putpixel(position.X + x, position.Y + y, MouseCursorBuffer[x + y * 16]);
                }
            }
        }
    }
}