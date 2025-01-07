extern "C" {
#include "../../drivers/vbe.h"
#include "../../shell/shell.h"
#include "../../drivers/filesystem/fat.h"
#include "../../memory/kmalloc.h"
#include "../../drivers/utils.h"
#include "../bitmap/bitmap.h"
}
#include "window.hpp"

inline void* operator new(size_t size) { return kmalloc(size); }

void Window::change_state(WindowState state) {
    this->state = state;
}

void Window::change_style(WindowStyle style) {
    this->style = style;
}

bool Window::move(int64_t x, int64_t y) {
    // De-render, move, render
    this->derender();
    this->x = x;
    this->y = y;
    this->render();
}

bool Window::resize(int64_t width, int64_t height) {
    // (If new size smaller) De-render, reallocate buffer, render
    if(this->width > width || this->height > height) this->derender();
    buffer = new uint32_t[width * height];
    render();
    return true;
}

bool Window::render() {
    if(this->buffer == nullptr) return false;
    for(int64_t y = 0; y < this->height; y++) {
        for(int64_t x = 0; x < this->width; x++) {
            vbe_putpixel(this->x + x, this->y + y, this->buffer[y * this->width + x]);
        }
    }
    return true;
}

bool Window::derender() {
    // Replace everything with the default background colour
    // This needs to be done in future by figuring out if
    // anything needs to be drawn that is below it.
    vbe_fillrect(this->x, this->y, this->width, this->height, 0x000000ff);
    return true;
}

bool Window::begin_drag() {
    if(drag_offset_x == -1 || drag_offset_y == -1) {
        return false;
    }
    this->is_dragging = true;
    return true;
}

bool Window::end_drag() {
    this->is_dragging = false;
    return true;
}

void Window::handle_mouse_move(int32_t x, int32_t y) {
    
}

void Window::handle_mouse_press() {

}

void Window::handle_key_pressed(char key) {

}