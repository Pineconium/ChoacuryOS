#include "window.hpp"
extern "C" {
#include "../../drivers/utils.h"
}

#pragma once
#ifndef GUI_HPP

namespace GUI {
    // Stuff for PSF1 fonts
    typedef struct {
        unsigned char magic[2];
        unsigned char mode;
        unsigned char charsize;
    } PSF1_HEADER;

    typedef struct {
        PSF1_HEADER* psf1_Header;
        void* glyphBuffer;
    } PSF1_FONT;

    extern unsigned char unifont[10294];

    struct uRect32 {
        //uRect32();
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
        //Rect32();
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
        //uPoint32();
        uPoint32(uint32_t x, uint32_t y) {
            this->x = x;
            this->y = y;
        }
    public:
        uint32_t x;
        uint32_t y;
    };

    struct Point32 {
        //Point32();
        Point32(int32_t x, int32_t y) {
            this->x = x;
            this->y = y;
        }
    public:
        int32_t x;
        int32_t y;
    };

    struct uCircle32 {
        uCircle32(uint32_t x, uint32_t y, uint32_t radius) {
            this->x = x;
            this->y = y;
            this->radius = radius;
        }
    public:
        uint32_t x;
        uint32_t y;
        uint32_t radius;
    };

    struct Circle32 {
        Circle32(int32_t x, int32_t y, int32_t radius) {
            this->x = x;
            this->y = y;
            this->radius = radius;
        }
    public:
        int32_t x;
        int32_t y;
        int32_t radius;
    };

    struct Bitmap {
        Bitmap(int32_t x, int32_t y, const uint8_t *data) {
            this->x = x;
            this->y = y;
            this->data = data;
        }
    public:
        int32_t x;
        int32_t y;
        const uint8_t *data; // Bitmap data (has to be unsigned because of what it is)
        
        // Needs resizing functionality, can come later (Same as uBitmap)
    };

    struct uBitmap {
        uBitmap(uint32_t x, uint32_t y, const uint8_t *data) {
            this->x = x;
            this->y = y;
            this->data = data;
        }
    public:
        uint32_t x;
        uint32_t y;
        const uint8_t *data; // Bitmap data

        // Needs resizing functionality, can come later
    };

    struct RIM {
        RIM(int32_t x, int32_t y, const uint8_t *data) {
            this->x = x;
            this->y = y;
            this->data = data;
        }
    public:
        int32_t x;
        int32_t y;
        const uint8_t *data; // RIM data

        // Needs resizing functionality, can come later
    };

    struct uRIM {
        uRIM(uint32_t x, uint32_t y, const uint8_t *data) {
            this->x = x;
            this->y = y;
            this->data = data;
        }
    public:
        uint32_t x;
        uint32_t y;
        const uint8_t *data; // RIM data

        // Needs resizing functionality, can come later
    };

    /// @brief Provides an area where text can be drawn to (Allows for multiline)
    struct uTextClip {

    };

    struct uText {
        uText(char* text, uint32_t x, uint32_t y, PSF1_FONT* font, uint32_t color) :
            text(text), x(x), y(y), font(font), color(color) { }
    public:
        char* text;
        uint32_t x;
        uint32_t y;
        uint32_t font_width = 8;
        uint32_t font_height = 20;
        uint32_t color;
        PSF1_FONT* font;
    };

    struct Text {
        Text(char* text, int32_t x, int32_t y, PSF1_FONT* font, int32_t color) :
            text(text), x(x), y(y), font(font), color(color) { }
    public:
        char* text;
        int32_t x;
        int32_t y;
        int32_t font_width = 8;
        int32_t font_height = 20;
        int32_t color;
        PSF1_FONT* font;
    };

    /// @brief Provides a structured buffer, with a buffer, width, and height
    struct Buffer {
        Buffer(uint32_t* buffer, uint32_t width, uint32_t height) :
            buffer(buffer), width(width), height(height) { }
    public:
        uint32_t* buffer;
        uint32_t width;
        uint32_t height;
    };

    // Buffer definitions
    /// @brief Clears a buffer with a certain colour
    /// @param buffer The buffer
    /// @param color The colour to clear it with
    void clear(Buffer buffer, uint32_t color);
    /// @brief Gets the pixel from a buffer at a certain position
    /// @param buffer The buffer
    /// @param point The point on the buffer
    /// @return The colour of the pixel
    uint32_t get_pixel(Buffer buffer, uPoint32 point);
    /// @brief Draws a pixel on a buffer at a certain position
    /// @param buffer The buffer
    /// @param point The point on the buffer
    /// @param color The colour
    void put_pixel(Buffer buffer, uPoint32 point, uint32_t color);
    /// @brief Draws a line from point a to point b
    /// @param buffer The buffer
    /// @param a Point A
    /// @param b Point B
    /// @param color The colour
    void draw_line(Buffer buffer, uPoint32 a, uPoint32 b, uint32_t color);
    /// @brief Draws a rectangle on the buffer
    /// @param buffer The buffer
    /// @param rect The rectangle
    /// @param color The colour
    void rect(Buffer buffer, uRect32 rect, uint32_t color);
    /// @brief Draws a filled rectangle on the buffer
    /// @param buffer The buffer
    /// @param rect The rectangle
    /// @param color The colour
    void fill_rect(Buffer buffer, uRect32 rect, uint32_t color);
    /// @brief Draws a bitmap to the buffer
    /// @param buffer The buffer
    /// @param bitmap The bitmap
    void draw_bitmap(Buffer buffer, uBitmap bitmap);
    /// @brief Draws a RIM to the buffer
    /// @param buffer The buffer
    /// @param rim The RIM
    /// @note Look at the documentation to see how to convert/edit RIM files
    void draw_rim(Buffer buffer, uRIM rim);
    /// @brief Draws a circle on the buffer
    /// @param buffer The buffer
    /// @param circle The circle
    void draw_circle(Buffer buffer, uCircle32 circle, uint32_t color);
    /// @brief Draws a filled circle on the buffer
    /// @param buffer The buffer
    /// @param circle The circle
    void draw_filled_circle(Buffer buffer, uCircle32 circle, uint32_t color);
    /// @brief Draws text on the buffer
    /// @param buffer The buffer
    /// @param text The text
    void draw_text(Buffer buffer, uText text);

    // Window definitions
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
    /// @brief Draws a bitmap to the window
    /// @param window The window
    /// @param bitmap The bitmap
    void draw_bitmap(Window* window, uBitmap bitmap);
    /// @brief Draws a RIM to the window
    /// @param window The window
    /// @param rim The RIM
    /// @note Look at the documentation to see how to convert/edit RIM files
    void draw_rim(Window* window, uRIM rim);
    /// @brief Draws a circle on the window
    /// @brief The window
    /// @param circle The circle
    void draw_circle(Window* window, uCircle32 circle, uint32_t color);
    /// @brief Draws a filled circle on the window
    /// @brief The window
    /// @param circle The circle
    void draw_filled_circle(Window* window, uCircle32 circle, uint32_t color);
    /// @brief Draws text on the window
    /// @param window The window
    /// @param text The text
    void draw_text(Window* window, uText text);

    /// @brief Draws part of the buffer to the screen
    /// @param buffer The buffer
    /// @param rect The part of the buffer to be rendered
    void vbe_render_part(Buffer buffer, uPoint32 point, uRect32 rect);
    /// @brief Draws the whole buffer to the screen
    /// @param buffer The buffer
    /// @param point The starting position
    void vbe_render_all(Buffer buffer, uPoint32 point);

    /// @brief Utilities to make it easier to do things, and also separates out a few things
    namespace Utils {
        /// @brief Draws a character to the position provided on the buffer
        /// @param buffer The buffer to draw the character on
        /// @param font The PSF1 font to draw the text with
        /// @param c The character
        /// @param x The top-left x position fo the character
        /// @param y The top-left y position of the character
        /// @param width The width of the font
        /// @param height The height of the font
        void text_draw_char(Buffer buffer, PSF1_FONT* font, char c, uint32_t x, uint32_t y,
            uint32_t width, uint32_t height, uint32_t color);

        /// @brief Draws a character to the position provided on the window's buffer
        /// @param window The window to draw the character on
        /// @param font The PSF1 font to draw the text with
        /// @param c The character
        /// @param x The top-left x position of the character
        /// @param y The top-left y position of the character
        /// @param width The width of the font
        /// @param height The height of the font
        void text_draw_char(Window* window, PSF1_FONT* font, char c, uint32_t x, uint32_t y,
            uint32_t width, uint32_t height, uint32_t color);
        /// @brief Converts raw data to a PSF1_FONT
        /// @param font The pointer to the PSF1_FONT
        /// @param data The pointer to the data
        void data_to_psf1(PSF1_FONT* font, void* data);
    }

    /// @brief Widgets namespace
    namespace Widgets {
        class UIWidget {
        public:
            UIWidget** children;
            uint32_t children_count = 0;
            void render(Window* window);
        };

        class Button : UIWidget {
        public:
            Button(uRect32 rect, char** text) : rect(rect), text(text) { }
            Button(uRect32 rect, const char** text) : rect(rect), text((char**)text) { }
            void render(Window* window);
            uRect32 rect;
            char** text;
            bool enabled = true;
            uint32_t normal_bg_colour;
            uint32_t hover_bg_colour;
            uint32_t click_bg_colour;
            uint32_t disabled_bg_colour;

            uint32_t normal_fg_colour;
            uint32_t hover_fg_colour;
            uint32_t click_fg_colour;
            uint32_t disabled_fg_colour;

            /// @brief The button state (0 = Normal, 1 = Hover, 2 = Clicked, 3 = Disabled)
            uint8_t state = 0;
        };
    }
}

#endif // GUI_HPP