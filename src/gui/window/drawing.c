#include "../../drivers/vbe.h"
#include "drawing.h"
#include "window.h"

uint32_t framebuffer_getpixel(uint32_t *framebuffer, uint32_t x, uint32_t y) {
    return framebuffer[y * 1920 /*todo: don't hardcode*/ + x];
}

void framebuffer_drawline(uint32_t *framebuffer, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        framebuffer_putpixel(framebuffer, x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void framebuffer_fillrect(uint32_t *framebuffer, uint32_t sx, uint32_t sy, uint32_t ex, uint32_t ey, uint32_t color) {
    for (uint32_t y = sy; y < ey; y++) {
        for (uint32_t x = sx; x < ex; x++) {
            framebuffer_putpixel(framebuffer, x, y, color);
        }
    }
}

void framebuffer_putpixel(uint32_t *framebuffer, u32 x, u32 y, u32 color) {
    u32 *pixel = &framebuffer[y * 1920 + x];

    u32 existing_color = *pixel;
    u8 alpha = (color >> 24) & 0xFF; // Extract alpha component from the color

    if (alpha == 0xFF) {
        // Fully transparent, do nothing
        return;
    } else if (alpha == 0x00) {
        // Fully opaque, overwrite
        *pixel = color & 0xFFFFFF; // Mask out alpha
    } else {
        // Blend the colors
        u8 src_r = (color >> 16) & 0xFF;
        u8 src_g = (color >> 8) & 0xFF;
        u8 src_b = color & 0xFF;

        u8 dst_r = (existing_color >> 16) & 0xFF;
        u8 dst_g = (existing_color >> 8) & 0xFF;
        u8 dst_b = existing_color & 0xFF;

        // Perform alpha blending
        u8 blended_r = ((src_r * (255 - alpha)) + (dst_r * alpha)) / 255;
        u8 blended_g = ((src_g * (255 - alpha)) + (dst_g * alpha)) / 255;
        u8 blended_b = ((src_b * (255 - alpha)) + (dst_b * alpha)) / 255;

        // Write the blended color back to the framebuffer
        *pixel = (blended_r << 16) | (blended_g << 8) | blended_b;
    }
}

uint32_t gui_window_getpixel(Window window, uint32_t x, uint32_t y) {
    return window.buffer[y * window.width + x];
}

void gui_window_drawline(Window window, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color) {
    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        gui_window_putpixel(window, x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void gui_window_fillrect(Window window, uint32_t sx, uint32_t sy, uint32_t ex, uint32_t ey, uint32_t color) {
    for (uint32_t y = sy; y < ey; y++) {
        for (uint32_t x = sx; x < ex; x++) {
            gui_window_putpixel(window, x, y, color);
        }
    }
}

void gui_window_putpixel(Window window, u32 x, u32 y, u32 color) {
    u32 *pixel = &window.buffer[y * window.width + x];

    u32 existing_color = *pixel;
    u8 alpha = (color >> 24) & 0xFF; // Extract alpha component from the color

    if (alpha == 0xFF) {
        // Fully transparent, do nothing
        return;
    } else if (alpha == 0x00) {
        // Fully opaque, overwrite
        *pixel = color & 0xFFFFFF; // Mask out alpha
    } else {
        // Blend the colors
        u8 src_r = (color >> 16) & 0xFF;
        u8 src_g = (color >> 8) & 0xFF;
        u8 src_b = color & 0xFF;

        u8 dst_r = (existing_color >> 16) & 0xFF;
        u8 dst_g = (existing_color >> 8) & 0xFF;
        u8 dst_b = existing_color & 0xFF;

        // Perform alpha blending
        u8 blended_r = ((src_r * (255 - alpha)) + (dst_r * alpha)) / 255;
        u8 blended_g = ((src_g * (255 - alpha)) + (dst_g * alpha)) / 255;
        u8 blended_b = ((src_b * (255 - alpha)) + (dst_b * alpha)) / 255;

        // Write the blended color back to the framebuffer
        *pixel = (blended_r << 16) | (blended_g << 8) | blended_b;
    }
}