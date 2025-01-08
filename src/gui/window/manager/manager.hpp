#include "../window.hpp"
extern "C" {
#include "../../../drivers/ps2_mouse.h"
#include "../../../drivers/ps2_keyboard.h"
}

class WindowManager {
private:
    Window* windows;
    size_t window_count;
    int32_t active_window; // -1 for none
public:
    /// @brief Handles a mouse event
    /// @param mouse_event The mouse event
    /// @param mouse_position The mouse position
    void handle_mouse_event(mouse_event_t mouse_event, int32_t mouse_x, int32_t mouse_y);
    /// @brief Handles a key event
    /// @param key_event The key event
    void handle_key_event(key_event_t key_event);
    /// @brief Registers a window
    /// @param window The window
    void register_window(Window* window);
};

class FallBackWM : WindowManager {
private:
    Window* windows;
    size_t window_count;
    int32_t active_window; // -1 for none
public:
    /// @brief Handles a mouse event
    /// @param mouse_event The mouse event
    /// @param mouse_position The mouse position
    void handle_mouse_event(mouse_event_t mouse_event, int32_t mouse_x, int32_t mouse_y);
    /// @brief Handles a key event
    /// @param key_event The key event
    void handle_key_event(key_event_t key_event);
    /// @brief Registers a window
    /// @param window The window
    void register_window(Window* window);
    /// @brief Renders the base fallback wm layout on the window buffer
    /// @param window The window
    void render_base(Window* window);
};