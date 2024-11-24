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

    void (*destroy)();
    void (*draw)();
    void (*update)();
    int (*move)(int64_t, int64_t);
    void (*maximise)();
    void (*minimise)();
    int (*resize)(int64_t, int64_t);
} Window;

void gui_window_render_titlebar(Window window);