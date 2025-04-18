#pragma once
#include "types.h"

// Source: http://cvs.savannah.nongnu.org/viewvc/*checkout*/vgabios/vgabios/vbe_display_api.txt?revision=1.14
#define VBE_DISPI_BANK_ADDRESS          0xA0000
#define VBE_DISPI_BANK_SIZE_KB          64

#define VBE_DISPI_MAX_XRES              1024
#define VBE_DISPI_MAX_YRES              768

#define VBE_DISPI_IOPORT_INDEX          0x01CE
#define VBE_DISPI_IOPORT_DATA           0x01CF

#define VBE_DISPI_INDEX_ID              0x0
#define VBE_DISPI_INDEX_XRES            0x1
#define VBE_DISPI_INDEX_YRES            0x2
#define VBE_DISPI_INDEX_BPP             0x3
#define VBE_DISPI_INDEX_ENABLE          0x4
#define VBE_DISPI_INDEX_BANK            0x5
#define VBE_DISPI_INDEX_VIRT_WIDTH      0x6
#define VBE_DISPI_INDEX_VIRT_HEIGHT     0x7
#define VBE_DISPI_INDEX_X_OFFSET        0x8
#define VBE_DISPI_INDEX_Y_OFFSET        0x9

#define VBE_DISPI_ID0                   0xB0C0
#define VBE_DISPI_ID1                   0xB0C1
#define VBE_DISPI_ID2                   0xB0C2
#define VBE_DISPI_ID3                   0xB0C3
#define VBE_DISPI_ID4                   0xB0C4

#define VBE_DISPI_DISABLED              0x00
#define VBE_DISPI_ENABLED               0x01
#define VBE_DISPI_VBE_ENABLED           0x40
#define VBE_DISPI_NOCLEARMEM            0x80

#define VBE_DISPI_LFB_PHYSICAL_ADDRESS  0xE0000000


#define VBE_DISPI_BPP_4   				0x04
#define VBE_DISPI_BPP_8   				0x08
#define VBE_DISPI_BPP_15  				0x0F
#define VBE_DISPI_BPP_16  				0x10
#define VBE_DISPI_BPP_24  				0x18
#define VBE_DISPI_BPP_32			    0x20
#define VBE_DISPI_LFB_ENABLED 			0x40

#define HD								1280,720
#define FHD 							1920,1080
#define UHD								3840,2160

typedef struct {
	unsigned char magic[2];
	unsigned char mode;
	unsigned char charsize;
} PSF1_HEADER;

typedef struct {
	PSF1_HEADER* psf1_Header;
	void* glyphBuffer;
} PSF1_FONT;

void BgaSetVideoMode(unsigned int Width, unsigned int Height, unsigned int BitDepth, int UseLinearFrameBuffer, int ClearVideoMemory);
void BgaSetBank(unsigned short BankNumber);
#ifdef __cplusplus
extern "C" {
#endif
void vbe_putpixel(u32 x, u32 y, u32 color);
#ifdef __cplusplus
}
#endif
void vbe_fillrect(u32 sx, u32 sy, u32 ex, u32 ey, u32 color);

void vbe_clear_screen(u8 color);
void vbe_drawline(u32 x1, u32 y1, u32 x2, u32 y2, u8 color);
u32 vbe_getpixel(u32 x, u32 y);
void putchar(int x, int y, char c, PSF1_FONT* font, uint32_t color);
void putchar_custom(int x, int y, char c, PSF1_FONT* font, uint32_t color, uint32_t bg_color, int font_width, int font_height);
void print(const char* str, PSF1_FONT* font, uint32_t color);
void reset_cursor();
void set_cursor(int x, int y);

void print_string(const char* str, u32 x, u32 y, u32 color);
void print_chr_line(unsigned int line, u32 x, u32 y, u32 color);