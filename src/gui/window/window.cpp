extern "C" {
#include "../../drivers/vbe.h"
#include "../../shell/shell.h"
#include "../../drivers/filesystem/fat.h"
#include "../../memory/kmalloc.h"
#include "../../drivers/utils.h"
#include "../bitmap/bitmap.h"
}
#include "window.hpp"

void Window::change_state(WindowState state) {

}

void Window::change_style(WindowStyle style) {

}

bool Window::move(int64_t x, int64_t y) {

}

bool Window::resize(int64_t width, int64_t height) {

}

bool Window::render() {

}

bool Window::derender() {

}

bool Window::begin_drag() {

}

bool Window::end_drag() {

}

void Window::handle_mouse_move(int32_t x, int32_t y) {

}

void Window::handle_mouse_press() {

}

void handle_key_pressed(char key) {
    
}