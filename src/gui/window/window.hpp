extern "C" {
#include <stdint.h>
}

#ifndef WINDOW_H
#define WINDOW_H

#define TITLEBAR_HEIGHT 30

typedef enum {
    Standard,
    None
} WindowStyle;

typedef enum {
    Minimized,  // Don't render
    Maximized,  // Render with a border but fullscreen (Check `WindoeStyle` first though)
    Fullscreen,
    Open        // Not Maximized nor fullscreen, just open
} WindowState;

class Window {
private:
    int64_t x;
    int64_t y;
    int64_t width;
    int64_t height;
    WindowStyle style;
    WindowState state;
    char** title;
    bool is_dragging;
public:
    /// @brief Window initializer
    /// @param _x Window position (X)
    /// @param _y Window position (Y)
    /// @param _width Window size (Width)
    /// @param _height Window size (height)
    /// @param _style Window style
    /// @param _state Windows state
    Window(int64_t _x, int64_t _y,
        int64_t _width, int64_t _height,
        WindowStyle _style, WindowState _state,
        char** _title) {
        x = _x;
        y = _y;
        width = _width;
        height = _height;
        style = _style;
        state = _state;
    }
    void change_state(WindowState state);
    void change_style(WindowStyle style);
    bool move(int64_t x, int64_t y);
    bool resize(int64_t width, int64_t height);
    bool render();
    bool derender();
    // Dragging functionality
    bool begin_drag();
    bool end_drag();
    // Mouse functionality
    void handle_mouse_move(int32_t x, int32_t y);
    void handle_mouse_press();
    // Keyboard functionality (Not implemented immidiately)
    void handle_key_pressed(char key);
};

namespace WindowManager {
    Window** windows;
    int32_t window_count;
    uint32_t focused_window;
}

#endif // WINDOW_H