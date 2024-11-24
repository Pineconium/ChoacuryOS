#include <stdint.h>

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
    uint32_t* buffer;

    void (*destroy)();
    void (*draw)(uint32_t*, int64_t, int64_t, int64_t, uint64_t);
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