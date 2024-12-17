#include "../../drivers/vbe.h"
#include "../../shell/shell.h"
#include "../../drivers/filesystem/fat.h"
#include "../../memory/kmalloc.h"
#include "../../drivers/utils.h"
#include "../bitmap/bitmap.h"
#include "drawing.h"
#include "window.h"

#define TITLEBAR_HEIGHT 30

void gui_window_render_titlebar(Window window) {
    /*vbe_fillrect(window.x, window.y, window.x + window.width, window.y + TITLEBAR_HEIGHT, 0x8A8A8A00); // Titlebar
    vbe_drawline(window.x, window.y, window.x, window.y + window.height, 0x8A8A8A00); // Top left -> Bottom left
    vbe_drawline(window.x, window.y, window.x + window.width, window.y, 0x8A8A8A00); // Top left -> Top right
    vbe_drawline(window.x, window.y + window.height, window.x + window.width, window.x + window.height, 0x8A8A8A00); // Bottom left -> Bottom right
    vbe_drawline(window.x + window.width, window.y, window.x + window.width, window.y + window.height, 0x8A8A8A00); // Top right -> Bottom right

    vbe_fillrect(window.x + 1, window.y + TITLEBAR_HEIGHT, window.x + window.width, window.y + window.height, 0x000000);

    //vbe_fillrect(window.x + window.width - (titlebar_height / 3), window.y + (titlebar_height / 3), window.x + window.width - (titlebar_height / 3) - 40, window.y + (titlebar_height / 3 * 2), 0xFF0000);
    vbe_fillrect(window.x + 5, window.y + 5, window.x + 20, window.y + 5, 0xFF0000);

    size_t title_length = sizeof(window.title);
    for(int i = 0; i < title_length; i++) {
        // 0xff is transparent
        putchar_custom(window.x + 3 + (9 * i), window.y + 3, window.title[i], font, 0xFFFFFF, 0xff, 8, 20);
    }

    FAT_file_t* test = FAT_OpenAbsolute(s_fat_fs, "/test.bmp");
    void* testData = (void*)kmalloc(sizeof(test->file_size));
    FAT_Read(testData, 0, 0, sizeof(test->file_size));
    draw_bitmap(window.x + 10, window.y + 10, testData);*/

    gui_window_fillrect(window, window.x, window.y, window.x + window.width, window.y + TITLEBAR_HEIGHT, 0x8A8A8A00); // Titlebar
    gui_window_drawline(window, window.x, window.y, window.x, window.y + window.height, 0x8A8A8A00); // Top left -> Bottom left
    gui_window_drawline(window, window.x, window.y, window.x + window.width, window.y, 0x8A8A8A00); // Top left -> Top right
    gui_window_drawline(window, window.x, window.y + window.height, window.x + window.width, window.x + window.height, 0x8A8A8A00); // Bottom left -> Bottom right
    gui_window_drawline(window, window.x + window.width, window.y, window.x + window.width, window.y + window.height, 0x8A8A8A00); // Top right -> Bottom right
}

void gui_window_render(Window window) {
    //gui_window_render_titlebar(window);
    //window.draw(window.buffer, window.x, window.y + TITLEBAR_HEIGHT, window.x + window.width, window.y + 30 + window.height);

    uint32_t *framebuffer = (uint32_t*)0xFD000000;

    for (size_t y = 0; y < window.height; y++)
    {
        for (size_t x = 0; x < window.width; x++)
        {
            int window_index = y * window.width + x;

            int framebuffer_index = (window.y + y) * 1920 + (window.x + x);

            // Set the pixel in the framebuffer
            framebuffer[framebuffer_index] = window.buffer[window_index];
        }
    }
}

void gui_window_initialise(Window window) {
    window.buffer = kmalloc(window.width * window.height * sizeof(uint32_t));
    memset(&window.buffer, 0xFFFFFF, sizeof(window.buffer));

    for (size_t y = 0; y < window.height; y++)
    {
        for (size_t x = 0; x < window.width; x++)
        {
            //framebuffer_putpixel(window.buffer, x, y, 0xff);
        }
    }
}

void gui_window_resize(Window window, int width, int height) {
    uint32_t* old_buffer = window.buffer;
    window.buffer = kmalloc(width * height * sizeof(uint32_t));
    for (size_t y = 0; y < window.height; y++)
    {
        
        for (size_t x = 0; x < window.height; x++)
        {
            window.buffer[y * window.width + x] = old_buffer[y * width + x];
        }
    }
    window.width = width;
    window.height = height;
}