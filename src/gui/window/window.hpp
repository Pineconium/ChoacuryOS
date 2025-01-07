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
    int32_t drag_offset_x = -1;
    int32_t drag_offset_y = -1;
    uint32_t* buffer = nullptr;
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

        buffer = new uint32_t[_width * _height];
    }
    /// @brief Changes the window state
    /// @param state The new window state
    void change_state(WindowState state);
    /// @brief Changes the window style
    /// @param style The new window style
    void change_style(WindowStyle style);
    /// @brief Moves the window to the new coordinates
    /// @param x The new X
    /// @param y The new Y
    /// @return If it was successful or not
    bool move(int64_t x, int64_t y);
    /// @brief Resizes the window
    /// @param width The new width
    /// @param height The new height
    /// @return Ifi t was successful or not
    bool resize(int64_t width, int64_t height);
    /// @brief Renders the window on the screen
    /// @return If it was successful or not
    bool render();
    /// @brief De-renders (Removes) the window from the screen
    /// @return If it was successful or not
    /// @note This only sets every pixel to 0x000000ff, needs to be done properly in future
    bool derender();
    // Dragging functionality
    /// @brief Sets the drag offset
    /// @param x The X drag offset
    /// @param y The Y drag offset
    void set_drag_offset(int32_t x, int32_t y);
    /// @brief Begins dragging, use set_drag_offset before or this will fail
    /// @returns If it was successful or not
    bool begin_drag();
    /// @brief Ends dragging
    /// @return If it was successful or not
    bool end_drag();
    // Mouse functionality
    /// @brief Handles a mouse move event
    /// @param x The X coordinate of the mouse
    /// @param y The Y coordinate of the mouse
    void handle_mouse_move(int32_t x, int32_t y);
    /// @brief Handles a mouse press event
    void handle_mouse_press();
    // Keyboard functionality (Not implemented immidiately)
    /// @brief Handles a keyboard key event
    /// @param key The key that was pressed
    void handle_key_pressed(char key);
};

namespace WindowManager {
    Window** windows;
    int32_t window_count;
    uint32_t focused_window;
}

#endif // WINDOW_H