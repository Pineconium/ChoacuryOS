#include "GUI.hpp"
#include "headers/bitmap.hpp"
#include "headers/rim.hpp"
#include "../../drivers/utils.h"

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

bool GUI::point_in_rect(uPoint32 point, uRect32 rect) {
    if(point.x >= rect.x && point.x < rect.x + rect.width) {
        if(point.y >= rect.y && point.y < rect.y + rect.height) {
            return true;
        }
    }
    return false;
}

void GUI::draw_bitmap(Window* window, uBitmap bitmap) {
    BITMAPFILEHEADER *file_header = (BITMAPFILEHEADER *)bitmap.data;
    BITMAPINFOHEADER *info_header = (BITMAPINFOHEADER *)(bitmap.data + sizeof(BITMAPFILEHEADER));

    // 4D42 is BM in HEX
    if(file_header->bfType != 0x4D42) {
        return;
    }

    int row_size = ((info_header->biBitCount * info_header->biWidth + 31) / 32) * 4;
    const uint8_t *pixel_data = bitmap.data + file_header->bfOffBits;

    for(int _y = 0; _y < abs(info_header->biHeight); _y++) {
        for(int _x = 0; _y < info_header->biWidth; _x++) {
            int idx = _y * row_size + _x * (info_header->biBitCount / 8);
            uint8_t g = pixel_data[idx + 2];
            uint8_t b = pixel_data[idx];
            uint8_t r = pixel_data[idx + 1];
            uint8_t a = 256; // Bitmaps don't support transparency, so just have this

            // RGB -> HEX                            Red                  Green               Blue         Alpha
            GUI::put_pixel(window, uPoint32(bitmap.x + _x, bitmap.y + _y), ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff) + ((a & 0xff) << 24));
        }
    }
}

void GUI::draw_rim(Window* window, uRIM rim) {
    RIMFILEHEADER *file_header = (RIMFILEHEADER *)rim.data;
    RIMINFOHEADER *info_header = (RIMINFOHEADER *)(rim.data + sizeof(RIMFILEHEADER));

    // 5249 is RI in HEX
	if (file_header->bfType != 0x5249) {
		return;
	}

    int row_size = ((info_header->biBitCount * info_header->biWidth + 31) / 32) * 4;
    const uint8_t *pixel_data = rim.data + file_header->bfOffBits;

    for(int _y = 0; _y < abs(info_header->biHeight); _y++) {
        for(int _x = 0; _y < info_header->biWidth; _x++) {
            int idx = _y * row_size + _x * (info_header->biBitCount / 8);
            uint8_t r = pixel_data[idx + 0];
            uint8_t g = pixel_data[idx + 1];
            uint8_t b = pixel_data[idx + 2];
            uint8_t a = pixel_data[idx + 3];

            // RGB -> HEX                            Red                  Green               Blue         Alpha
            GUI::put_pixel(window, uPoint32(rim.x + _x, rim.y + _y), ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff) + ((a & 0xff) << 24));
        }
    }
}

void GUI::draw_circle(Window* window, uCircle32 circle, uint32_t color) {
    int width = circle.radius * 2;
    int height = circle.radius * 2;

    for(uint32_t angle = 0; angle < 360; angle++) {
        int x = (circle.radius * cos(angle * M_PI / 180));
        int y = (circle.radius * sin(angle * M_PI / 180));

        int cx = height / 2 + x;
        int cy = width / 2 + y;

        put_pixel(window, uPoint32(circle.x + cx, circle.y + cy), color);
    }

    GUI::draw_line(window, uPoint32(circle.x, circle.y), uPoint32(circle.x + (circle.radius * 2), circle.y + (circle.radius * 2)), 0xFFFFFF);
}

void GUI::draw_filled_circle(Window* window, uCircle32 circle, uint32_t color) {
    uint32_t center_x = circle.x + circle.radius;
    uint32_t center_y = circle.y + circle.radius;

    for(uint32_t y = -circle.radius; y <= circle.radius; y++) {
        for(uint32_t x = -circle.radius; x <= circle.radius; x++) {
            if((x * x) * (y * y) <= (circle.radius * circle.radius)) {
                put_pixel(window, uPoint32(circle.x + x, circle.y + y), color);
            }
        }
    }

    GUI::draw_line(window, uPoint32(circle.x, circle.y), uPoint32(circle.x + (circle.radius * 2), circle.y + (circle.radius * 2)), 0xFFFFFF);
}