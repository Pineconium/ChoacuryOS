#include "../window.hpp"
#include "manager.hpp"
#include "../gui.hpp"

using namespace GUI;

void FallBackWM::register_window(Window* window) {
    this->windows[this->window_count + 1] = *window;
    this->window_count++;
}

void FallBackWM::render_base(Window* window) {
    int titlebar_height = 30;

    fill_rect(window, uRect32(0, 0, window->width, titlebar_height), 0x8A8A8A00);
    rect(window, uRect32(0, 0, window->width, window->height), 0x8A8A8A00);

    fill_rect(window, uRect32(window->width - 25, 5, 20, 20), 0xFF0000);
    fill_rect(window, uRect32(window->width - (25 + 5 + 25), 5, 20, 20), 0xFFFF00);
    fill_rect(window, uRect32(window->width - (25 + 5 + 25 + 5 + 25), 5, 20, 20), 0x00FF00);

    // Testing
    draw_line(window, uPoint32(0, 30), uPoint32(500, 500), 0xFFFFFF);
    rect(window, uRect32(10, 10 + 30, 100, 100), 0xFF0000);
    fill_rect(window, uRect32(10 + 110, 10 + 30, 100, 100), 0xFF0000);
    draw_circle(window, uCircle32(10, 10 + 30, 50), 0x00FF00);
    draw_filled_circle(window, uCircle32(10 + 110, 10 + 30, 50), 0x00FF00);
}

void FallBackWM::handle_mouse_event(mouse_event_t mouse_event, int32_t mouse_x, int32_t mouse_y) {
    int titlebar_height = 30;
    Window* active_window = &this->windows[this->active_window];

    if(mouse_event.type == MOUSE_MOVE_EVENT) {
        if(mouse_x >= active_window->x && mouse_x < active_window->x + active_window->width) {
            if(mouse_y >= active_window->y && mouse_y < active_window->y + active_window->height) {
                active_window->handle_mouse_move(mouse_x, mouse_y);
            }
        }
    } else if(mouse_event.type == MOUSE_BUTTON_EVENT) {
        if(mouse_event.button_event.button == mouse_button_t::LEFT) {
            if(mouse_x >= active_window->x + active_window->width - 25 &&
                mouse_x < active_window->x + active_window->width - 5) {
                if(mouse_y >= active_window->y + 5 &&
                    mouse_y < active_window->y + 25) {
                        active_window->~Window();
                        // Unregister window
                    }
            } else if(GUI::point_in_rect(uPoint32(mouse_x, mouse_y), uRect32(active_window->width - (25 + 5 + 25), 5, 20, 20))) {
                if(active_window->state == WindowState::Open) {
                    active_window->change_state(WindowState::Maximized);
                } else {
                    active_window->change_state(WindowState::Open);
                }
            } else {
                if(mouse_x >= active_window->x && mouse_x < active_window->x + active_window->width) {
                    if(mouse_y >= active_window->y && mouse_y < active_window->y + titlebar_height) {
                        if(active_window->is_dragging) {
                            active_window->end_drag();
                        } else {
                            active_window->set_drag_offset(active_window->x - mouse_x, active_window->y - mouse_y);
                            active_window->begin_drag();
                        }
                    }
                }
            }
        }
    }
}

void FallBackWM::make_window_active(uint32_t index) {
    this->active_window = index;
}