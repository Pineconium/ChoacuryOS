#include "GUI.hpp"

void GUI::clear(Window* window, uint32_t color) {
    for (uint32_t y = 0; y < window->height; y++) {
        for (uint32_t x = 0; x < window->width; x++) {
            GUI::put_pixel(window, GUI::uPoint32(x, y), color);
        }
    }
}

uint32_t GUI::get_pixel(Window* window, uPoint32 point) {
    return window->buffer[point.y * window->width + point.x];
}

void GUI::put_pixel(Window* window, uPoint32 point, uint32_t color) {
    uint32_t *pixel = &window->buffer[point.y * window->width + point.x];

    uint32_t existing_color = *pixel;
    uint8_t alpha = (color >> 24) & 0xFF; // Extract alpha component from the color

    if (alpha == 0xFF) {
        // Fully transparent, do nothing
        return;
    } else if (alpha == 0x00) {
        // Fully opaque, overwrite
        *pixel = color & 0xFFFFFF; // Mask out alpha
    } else {
        // Blend the colors
        uint8_t src_r = (color >> 16) & 0xFF;
        uint8_t src_g = (color >> 8) & 0xFF;
        uint8_t src_b = color & 0xFF;

        uint8_t dst_r = (existing_color >> 16) & 0xFF;
        uint8_t dst_g = (existing_color >> 8) & 0xFF;
        uint8_t dst_b = existing_color & 0xFF;

        // Perform alpha blending
        uint8_t blended_r = ((src_r * (255 - alpha)) + (dst_r * alpha)) / 255;
        uint8_t blended_g = ((src_g * (255 - alpha)) + (dst_g * alpha)) / 255;
        uint8_t blended_b = ((src_b * (255 - alpha)) + (dst_b * alpha)) / 255;

        // Write the blended color back to the framebuffer
        *pixel = (blended_r << 16) | (blended_g << 8) | blended_b;
    }
}

void GUI::draw_line(Window* window, uPoint32 a, uPoint32 b, uint32_t color) {
    uint32_t x1 = a.x;
    uint32_t y1 = a.y;
    uint32_t x2 = b.x;
    uint32_t y2 = b.y;

    int dx = abs(x2 - x1), sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1), sy = y1 < y2 ? 1 : -1;
    int err = dx + dy, e2;

    while (1) {
        GUI::put_pixel(window, uPoint32(x1, y1), color);
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) { err += dy; x1 += sx; }
        if (e2 <= dx) { err += dx; y1 += sy; }
    }
}

void GUI::rect(Window* window, uRect32 rect, uint32_t color) {
    uint32_t sx = rect.x;
    uint32_t sy = rect.y;
    uint32_t ex = rect.x + rect.width;
    uint32_t ey = rect.y + rect.height;

    draw_line(window, uPoint32(sx, sy), uPoint32(ex, sy), color); // Top line
    draw_line(window, uPoint32(ex, sy), uPoint32(ex, ey), color); // Right line
    draw_line(window, uPoint32(sx, ey), uPoint32(ex, ey), color); // Bottom line
    draw_line(window, uPoint32(ex, ey), uPoint32(sx, ey), color); // Left line
}

void GUI::fill_rect(Window* window, uRect32 rect, uint32_t color) {
    uint32_t sx = rect.x;
    uint32_t sy = rect.y;
    uint32_t ex = rect.x + rect.width;
    uint32_t ey = rect.y + rect.height;
    for (uint32_t y = sy; y < ey; y++) {
        for (uint32_t x = sx; x < ex; x++) {
            GUI::put_pixel(window, uPoint32(x, y), color);
        }
    }
}