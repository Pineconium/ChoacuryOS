#include <stdint.h>

#define TITLEBAR_HEIGHT 30

typedef enum {
    Standard,
    None
} WindowStyle;

typedef struct {
    int64_t x;
    int64_t y;
    uint64_t width;
    uint64_t height;
    WindowStyle style;
    char* title;
    int should_draw; // 1 = Yes, 0 = No
    int is_dragging;
    int drag_offset_x;
    int drag_offset_y;

    void (*destroy)();
    void (*draw)(int64_t, int64_t, int64_t, uint64_t);
    void (*update)();
    int (*move)(int64_t, int64_t);
    void (*maximise)();
    void (*minimise)();
    int (*resize)(int64_t, int64_t);
} Window;

void gui_window_initialise(Window window);
void gui_window_resize(Window window, int width, int height);
void gui_window_render_titlebar(Window window);
void gui_window_render(Window window); // This will render the titlebar and then the draw function
int gui_window_move(Window* window, int64_t new_x, int64_t new_y);

// Drawing.h
//uint32_t gui_window_getpixel(Window window, uint32_t x, uint32_t y);
//void gui_window_drawline(Window window, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t color);
//void gui_window_fillrect(Window window, uint32_t sx, uint32_t sy, uint32_t ex, uint32_t ey, uint32_t color);
//void gui_window_putpixel(Window window, u32 x, u32 y, u32 color);

// Manager.h
static Window** windows[15];
static Window* active_window = NULL;

void gui_window_manager_register_window(Window window);
int gui_window_manager_mouse_over_titlebar(Window* window, int mouse_x, int mouse_y);
void gui_window_manager_handle_mouse_move(int mouse_x, int mouse_y);