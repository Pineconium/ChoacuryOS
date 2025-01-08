#include "window.hpp"
extern "C" {
#include "../../drivers/utils.h"
}

namespace GUI {
    struct uRect32 {
        uRect32(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
        }
    public:
        uint32_t x;
        uint32_t y;
        uint32_t width;
        uint32_t height;
    };

    struct Rect32 {
        Rect32(int32_t x, int32_t y, int32_t width, int32_t height) {
            this->x = x;
            this->y = y;
            this->width = width;
            this->height = height;
        }
    public:
        int32_t x;
        int32_t y;
        int32_t width;
        int32_t height;
    };

    struct uPoint32 {
        uPoint32(uint32_t x, uint32_t y) {
            this->x = x;
            this->y = y;
        }
    public:
        uint32_t x;
        uint32_t y;
    };

    struct Point32 {
        Point32(int32_t x, int32_t y) {
            this->x = x;
            this->y = y;
        }
    public:
        int32_t x;
        int32_t y;
    };

    /// @brief Clears a window with a certain colour
    /// @param window The window
    /// @param color The colour to clear it with
    void clear(Window* window, uint32_t color);
    /// @brief Gets the pixel from a window at a certain position
    /// @param window The window
    /// @param point The point on the window
    /// @return The colour of the pixel
    uint32_t get_pixel(Window* window, uPoint32 point);
    /// @brief Draws a pixel on a window at a certain position
    /// @param window The window
    /// @param point The point on the window
    /// @param color The colour
    void put_pixel(Window* window, uPoint32 point, uint32_t color);
    /// @brief Draws a line from point a to point b
    /// @param window The window
    /// @param a Point A
    /// @param b Point B
    /// @param color The colour
    void draw_line(Window* window, uPoint32 a, uPoint32 b, uint32_t color);
    /// @brief Draws a rectangle on the window
    /// @param window The window
    /// @param rect The rectangle
    /// @param color The colour
    void rect(Window* window, uRect32 rect, uint32_t color);
    /// @brief Draws a filled rectangle on the window
    /// @param window The window
    /// @param rect The rectangle
    /// @param color The colour
    void fill_rect(Window* window, uRect32 rect, uint32_t color);
    /// @brief Sees if a point is in a rectangle
    /// @param point The point
    /// @param rect The rectangle
    /// @return Returns true if the point is in the rectangle
    bool point_in_rect(uPoint32 point, uRect32 rect);
}