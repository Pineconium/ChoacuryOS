/* now we can have better graphics */

#include "vbe.h"
#include "types.h"
#include "../memory/kmalloc.h"



/* move this somewhere else */
static inline void outpw(u16 port, u16 value) {
    __asm__ __volatile__ ("outw %0, %1" : : "a"(value), "Nd"(port));
}

static inline u16 inpw(u16 port) {
    u16 value;
    __asm__ __volatile__ ("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}
// End



#define ADDRESS 0xFD000000
#define FONT_WIDTH 8
#define FONT_HEIGHT 16
int VBE_WIDTH  = 1920;
int VBE_HEIGHT = 1080;
static int cursor_x = 0;
static int cursor_y = 0;

void print(const char* str, PSF1_FONT* font, uint32_t color) {
    while (*str) {
        if (*str == '\n') {
            cursor_x = 0;
            cursor_y += FONT_HEIGHT;
        } else if (*str == '\r') {
            cursor_x = 0;
        } else {
            putchar(cursor_x, cursor_y, *str, font, color);
            cursor_x += FONT_WIDTH;
            if (cursor_x >= VBE_WIDTH) {
                cursor_x = 0;
                cursor_y += FONT_HEIGHT;
            }
        }
        str++;

        if (cursor_y >= VBE_HEIGHT) {
            cursor_y = 0;  // Reset to the top if needed
        }
    }
}

void vbe_clear_screen(u8 color) {
    for (u32 y = 0; y < VBE_HEIGHT; y++) {
        for (u32 x = 0; x < VBE_WIDTH; x++) {
            vbe_putpixel(x,y,color);
        }
    }
}

// Reset the cursor position to the top-left corner of the screen
void reset_cursor() {
    cursor_x = 0;
    cursor_y = 0;
}

void set_cursor(int x, int y) {
    cursor_x = x;
    cursor_y = y;
}

void putchar(int x, int y, char c, PSF1_FONT* font, uint32_t color) {
    uint8_t* glyph = (uint8_t*)font->glyphBuffer + (c * font->psf1_Header->charsize);

    for (int row = 0; row < FONT_HEIGHT; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < FONT_WIDTH; col++) {
            if (bits & (0x80 >> col)) {
                vbe_putpixel(x + col, y + row, color);
            } else {
                vbe_putpixel(x + col, y + row, 0x000000);  // Ensure the background is black
            }
        }
    }
}
/*
u8 vga_getpixel(u32 x, u32 y) {
    if (x < VBE_WIDTH && y < VBE_HEIGHT) {
        return ADDRESS[y * VBE_WIDTH + x];
    }
    return 0;
}
*/

u32 vbe_getpixel(u32 x, u32 y) {
	u32 *framebuffer = (u32*)ADDRESS;
    return framebuffer[y * 1920 /*todo: don't hardcode*/ + x];
}

void vbe_drawline(u32 x1, u32 y1, u32 x2, u32 y2, u8 color) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        vbe_putpixel(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void vbe_fillrect(u32 sx, u32 sy, u32 ex, u32 ey, u32 color) {
    for (u32 y = sy; y < ey; y++) {
        for (u32 x = sx; x < ex; x++) {
            vbe_putpixel(x, y, color);
        }
    }
}

// DONT TRY THIS IN BOCHS! BOCHS IGNORES BANK ADDRESS WRITES WHEN
// LFB IS ACTIVE
void vbe_putpixel(u32 x, u32 y, u32 color) {
	u32 *framebuffer = (u32*)ADDRESS;
    framebuffer[y * VBE_WIDTH + x] = color;
}


void BgaWriteRegister(unsigned short IndexValue, unsigned short DataValue){
    outpw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    outpw(VBE_DISPI_IOPORT_DATA, DataValue);
}

unsigned short BgaReadRegister(unsigned short IndexValue){
    outpw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    return inpw(VBE_DISPI_IOPORT_DATA);
}

int BgaIsAvailable(void){
    return (BgaReadRegister(VBE_DISPI_INDEX_ID) == VBE_DISPI_ID4);
}

void BgaSetVideoMode(unsigned int Width, unsigned int Height, unsigned int BitDepth, int UseLinearFrameBuffer, int ClearVideoMemory){
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    BgaWriteRegister(VBE_DISPI_INDEX_XRES, Width);
    BgaWriteRegister(VBE_DISPI_INDEX_YRES, Height);
    BgaWriteRegister(VBE_DISPI_INDEX_BPP, BitDepth);
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED |
        (UseLinearFrameBuffer ? VBE_DISPI_LFB_ENABLED : 0) |
        (ClearVideoMemory ? 0 : VBE_DISPI_NOCLEARMEM));
	VBE_WIDTH = Width;
	VBE_HEIGHT = Height;
}

void BgaSetBank(unsigned short BankNumber){
    BgaWriteRegister(VBE_DISPI_INDEX_BANK, BankNumber);
}










