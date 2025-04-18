#include "../gui/window/gui.hpp"
extern "C" {
#include "../memory/kmalloc.h"
}

/// @brief A hidden pong game in the recovery mode
class Pong {
    GUI::Buffer game_buffer = GUI::Buffer((uint32_t*)kmalloc(1920 * 108 * sizeof(uint32_t)), 1920, 1080); // Centre area (Where the circle is)

    uint16_t score_bot = 0; // Score always starts at 0
    uint16_t score_player = 0;
    uint8_t circle_radius = 50;
    GUI::uPoint32 ball = GUI::uPoint32((1920 / 2) - circle_radius, (1080 / 2) - circle_radius); // The ball starts at the centre
    uint8_t ball_direction = 180; // 0-360, 0 = toward the bot, 180 = toward the player
    uint8_t ball_speed = 10;

    // These positions are the middle of the paddle
    uint8_t bot_pos = 1080 / 2;
    uint8_t player_pos = 1080 / 2;
    // These are 100 pixels high, and 25 pixels wide, 10 padding on either side
    uint8_t paddle_height = 100;
    uint8_t paddle_width = 25;

    // Debug mode adds pink boxes as the background on surfaces that are drawn
    // It also adds green boxes around surfaces that are collidable
    // It also adds blue boxes around surfaces that bounce
    // It also adds red boxes around surfaces that mean game over
    bool debug = false;

    // Some basic keyboard locking, so the input doesn't get spammed
    uint8_t iteration = 0;
    bool input_locked = false;
    uint64_t tick = 0;

    // Pausing and redrawing
    bool paused = true;
    bool started = false;
public:
    Pong() {
        //GUI::clear(game_buffer, 0x00000000);
        //GUI::vbe_render_all(game_buffer, GUI::uPoint32(0, 0));
    }
    ~Pong() {
        kfree(&game_buffer); // Free the memory
    }
    Pong(GUI::Buffer buffer) : game_buffer(buffer) { }
    void init_pong();
    void update_paddle_pos(uint8_t last_pos, uint8_t new_pos, uint8_t side);
    void update_ball_pos(GUI::uPoint32 last_pos, GUI::uPoint32 new_pos);
    void simulate_ball(); // Get it
    void simulate_bot();

    void draw_main(); // Things like the line down the middle, and score at the top
    void draw_about(); // Box in the middle with some text
};