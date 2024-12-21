#include <stddef.h>
#include "../window.h"

void gui_window_manager_register_window(Window window) {
    for (int32_t i = 0; i < sizeof(windows); i++)
    {
        if(*windows[i] == NULL) {
            *windows[i] = &window;
            active_window = &window;
            gui_window_render(window);
            return 1;
        }
    }
    return 0;
}

int gui_window_manager_mouse_over_titlebar(Window* window, int mouse_x, int mouse_y) {
    if(mouse_x >= window->x && mouse_x <= window->x + window->width) {
        if(mouse_y >= window->y && mouse_y <= window->y + TITLEBAR_HEIGHT) {
            return 1;
        }
    }
    return 0;
}

void gui_window_manager_handle_mouse_move(int mouse_x, int mouse_y) {
    if(active_window != NULL) {
        if(active_window->is_dragging) {
            int64_t new_x = mouse_x - active_window->drag_offset_x;
            int64_t new_y = mouse_y - active_window->drag_offset_y;

            if(active_window->move) {
                active_window->move(new_x, new_y);
            }
        }
    }
}