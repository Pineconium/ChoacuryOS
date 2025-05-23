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

Window::~Window() {
    this->derender();
    kfree(this);
}

void Window::change_state(WindowState state) {
    this->state = state;
    if(state == WindowState::Maximized) {
        this->mm_x = this->x;
        this->mm_y = this->y;
        this->mm_width = this->width;
        this->mm_height = this->height;

        // This part needs to be more dynamic when we get mutliple sizes for vbe.c
        this->x = 0;
        this->y = 0;
        this->width = 1920;
        this->height = 1080;
    } else {
        this->x = this->mm_x;
        this->y = this->mm_y;
        this->width = this->mm_width;
        this->height = this->mm_height;
    }
    this->derender();
    this->render();
}

void Window::change_style(WindowStyle style) {
    this->style = style;
}

void Window::set_drag_offset(int32_t x, int32_t y) {
    this->drag_offset_x = x;
    this->drag_offset_y = y;
}

bool Window::move(int64_t x, int64_t y) {
    // De-render, move, render
    this->derender();
    this->x = x;
    this->y = y;
    this->render();
    return true;
}

bool Window::resize(int64_t width, int64_t height) {
    // (If new size smaller) De-render, reallocate buffer, render
    if(this->width > width || this->height > height) this->derender();
    // Initialize the buffer
    buffer = (uint32_t*)kmalloc(width * height * sizeof(uint32_t));

    // Clear it (Sometimes random data can be left behind)
    for(int64_t y = 0; y < this->height; y++) {
        for(int64_t x = 0; x < this->width; x++) {
            // Clear the buffer with black
            this->buffer[y * this->width + x] = 0x0;
        }
    }
    
    // Tell it to render
    render();
    // Return success
    return true;
}

bool Window::render() {
    // If the buffer is empty, do nothing
    if(this->buffer == nullptr) return false;
    // Draw each pixel on the screen
    for(int64_t y = 0; y < this->height; y++) {
        for(int64_t x = 0; x < this->width; x++) {
            vbe_putpixel(this->x + x, this->y + y, this->buffer[y * this->width + x]);
        }
    }
    // Return success
    return true;
}

bool Window::render_part(int32_t _x, int32_t _y, int32_t width, int32_t height) {
    // If the buffer is empty, do nothing
    if(this->buffer == nullptr) return false;
    if(_x > this->width || _x < 0 || _y > this->height || _y < 0) return false;

    // Draw each pixel on the screen
    for(int64_t y = _y; y < height; y++) {
        for(int64_t x = _x; x < width; x++) {
            vbe_putpixel(this->x + x, this->y + y, this->buffer[y * this->width + x]);
        }
    }
    // Return success
    return true;
}

bool Window::derender() {
    // Replace everything with the default background colour
    // This needs to be done in future by figuring out if
    // anything needs to be drawn that is below it.
    vbe_fillrect(this->x, this->y, this->x + this->width, this->y + this->height, 0x000000ff);
    return true;
}

bool Window::begin_drag() {
    // This needs to be set (Even if it's set to 0)
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
    if(this->is_dragging) {
        this->move(x + this->drag_offset_x, y + this->drag_offset_y);
    }
}

void Window::handle_mouse_press() {
    // For anything like buttons
    // (You can use set_drag_offset, which SHOULD be set when the mouse goes over the window)
}

void Window::handle_key_pressed(char key) {
    // No keyboard functionality as of now
}