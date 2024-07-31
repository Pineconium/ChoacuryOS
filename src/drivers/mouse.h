#pragma once

typedef enum {
    LEFT,
    RIGHT,
    MIDDLE,
    EXTRA1,
    EXTRA2,
} mouse_button_t;

typedef struct {
    bool pressed;
    mouse_button_t button;
} mouse_button_event_t;

typedef struct {
    int rel_x;
    int rel_y;
} mouse_move_event_t;

typedef struct {
    int value;
} mouse_scroll_event_t;

typedef enum {
    MOUSE_EVENT_NONE,
    MOUSE_BUTTON_EVENT,
    MOUSE_MOVE_EVENT,
    MOUSE_SCROLL_EVENT,
} mouse_event_type_t;

typedef struct {
    mouse_event_type_t type;
    union {
        mouse_button_event_t button_event;
        mouse_move_event_t move_event;
        mouse_scroll_event_t scroll_event;
    };
} mouse_event_t;
