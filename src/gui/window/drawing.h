#include "../../drivers/vbe.h"
//#include "window.h"

uint32_t framebuffer_getpixel(uint32_t *framebuffer, uint32_t x, uint32_t y);
void framebuffer_drawline(uint32_t *framebuffer, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color);
void framebuffer_fillrect(uint32_t *framebuffer, uint32_t sx, uint32_t sy, uint32_t ex, uint32_t ey, uint32_t color);
void framebuffer_putpixel(uint32_t *framebuffer, u32 x, u32 y, u32 color);