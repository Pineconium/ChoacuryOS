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

    void clear(Window* window, uint32_t color);
    uint32_t get_pixel(Window* window, uPoint32 point);
    void put_pixel(Window* window, uPoint32 point, uint32_t color);
    void draw_line(Window* window, uPoint32 a, uPoint32 b, uint32_t color);
    void rect(Window* window, uRect32 rect, uint32_t color);
    void fill_rect(Window* window, uRect32 rect, uint32_t color);
}