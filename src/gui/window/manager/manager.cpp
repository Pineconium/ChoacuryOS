#include "../window.hpp"
#include "manager.hpp"
#include "../gui.hpp"

using namespace GUI;

void FallBackWM::register_window(Window* window) {
    
}

void FallBackWM::render_base(Window* window) {
    int titlebar_height = 30;

    fill_rect(window, uRect32(0, 0, window->width, titlebar_height), 0x8A8A8A00);
    rect(window, uRect32(0, 0, window->width, window->height), 0x8A8A8A00);

    fill_rect(window, uRect32(window->width - 25, 5, 20, 20), 0xFF0000);
}