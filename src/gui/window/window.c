#include "../../drivers/vbe.h"
#include "../../shell/shell.h"
#include "../../drivers/filesystem/fat.h"
#include "../../memory/kmalloc.h"
#include "../../drivers/utils.h"
#include "../bitmap/bitmap.h"
#include "window.h"

//#define TITLEBAR_HEIGHT 30 // Moved to window.h

void gui_window_render_titlebar(Window window) {
    vbe_fillrect(window.x, window.y, window.x + window.width, window.y + TITLEBAR_HEIGHT, 0x8A8A8A00); // Titlebar
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
    draw_bitmap(window.x + 10, window.y + 10, testData);
}

void gui_window_render(Window window) {
    gui_window_render_titlebar(window);
    window.draw(window.x, window.y + TITLEBAR_HEIGHT, window.x + window.width, window.y + 30 + window.height);
}

int gui_window_move(Window* window, int64_t new_x, int64_t new_y) {
    window->x = new_x;
    window->y = new_y;

    gui_window_render(*window);

    return 1;
}

void gui_window_initialise(Window window) {

}

void gui_window_resize(Window window, int width, int height) {

}