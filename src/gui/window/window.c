#include "../../drivers/vbe.h"
#include "../../shell/shell.h"
#include "window.h"

void gui_window_render_titlebar(Window window) {
    int titlebar_height = 30;
    vbe_fillrect(window.x, window.y, window.x + window.width, window.y + titlebar_height, 0x8A8A8A00); // Titlebar
    vbe_drawline(window.x, window.y, window.x, window.y + window.height, 0x8A8A8A00); // Top left -> Bottom left
    vbe_drawline(window.x, window.y, window.x + window.width, window.y, 0x8A8A8A00); // Top left -> Top right
    vbe_drawline(window.x, window.y + window.height, window.x + window.width, window.x + window.height, 0x8A8A8A00); // Bottom left -> Bottom right
    vbe_drawline(window.x + window.width, window.y, window.x + window.width, window.y + window.height, 0x8A8A8A00); // Top right -> Bottom right

    vbe_fillrect(window.x + 1, window.y + titlebar_height, window.x + window.width, window.y + window.height, 0x000000);

    //vbe_fillrect(window.x + window.width - (titlebar_height / 3), window.y + (titlebar_height / 3), window.x + window.width - (titlebar_height / 3) - 40, window.y + (titlebar_height / 3 * 2), 0xFF0000);
    vbe_fillrect(window.x + 5, window.y + 5, window.x + 20, window.y + 5, 0xFF0000);

    size_t title_length = sizeof(window.title);
    for(int i = 0; i < title_length; i++) {
        putchar_custom(window.x + 3 + (9 * i), window.y + 3, window.title[i], font, 0xFFFFFF, 20, 8);
    }
}