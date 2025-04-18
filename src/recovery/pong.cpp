// Pong because why not
// (Barely works)

#include "pong.hpp"
extern "C" {
#include "../gui/Point.h"
#include "../drivers/ps2_keyboard.h"
//#include "../drivers/ps2_mouse.h" // Not needed for now
#include "../drivers/vbe.h"
#include "../memory/kmalloc.h"
}
#include "../gui/window/window.hpp"
#include "../gui/window/manager/manager.hpp"
#include "../gui/window/gui.hpp"
#include "../gui/desktop.h"
#include "../memory/pmm.h"
#include "../shell/shell.h"
#include "../shell/terminal.h"

using namespace GUI;

void Pong::init_pong() {
    GUI::clear(game_buffer, 0x00000000); // Clear the buffer with black
    GUI::vbe_render_all(game_buffer, GUI::uPoint32(0, 0));
    
    draw_main();

    //GUI::clear(game_buffer, 0x00000000); // Clear the buffer with black
    //update_ball_pos(ball, ball);
    simulate_ball();

    //GUI::uPoint32 pos(960, 540);
	//draw_filled_circle(game_buffer, GUI::uCircle32(pos.x, pos.y, circle_radius), 0x00ffffff);
	//GUI::vbe_render_part(game_buffer, pos, GUI::uRect32(pos.x, pos.y, circle_radius * 2, circle_radius * 2));

    //GUI::vbe_render_all(game_buffer, GUI::uPoint32(0, 0));

    // Spawn paddles
    //GUI::clear(game_buffer, 0x00ff00ff); // Test
    update_paddle_pos(bot_pos, 100, 0);
    update_paddle_pos(player_pos, 100, 1);

    //vbe_render_all(game_buffer, uPoint32(0, 0));

    draw_about();

    for(;;) {
        tick++;
        iteration++;

        key_event_t key_event;
        ps2_get_key_event(&key_event);

        // Unlock input if it's locked
        if(input_locked) {
            if(iteration >= 1) {
                iteration = 0;
                input_locked = false;
            } else continue;
        } else {
            if(key_event.key == KEY_Escape) break; // Go back
            else if(key_event.key == KEY_Space) {
                paused = !paused;
                if(paused) draw_about();
                else {
                    // Redraw everything
                    if(started == false) {
                        bot_pos = 10;
                        player_pos = 10;
                        ball = uPoint32((1920 / 2) - circle_radius, (1080 / 2) - circle_radius);
                        started = true;
                        paused = false;
                    }
                    clear(game_buffer, 0x00000000);
                    //vbe_render_all(game_buffer, uPoint32(0, 0));
                    draw_main();
                    update_ball_pos(ball, ball);
                    update_paddle_pos(bot_pos, bot_pos, 0);
                    //update_paddle_pos(player_pos, player_pos, 1);
                }
            } else if(key_event.key == KEY_ArrowUp) {
                if(player_pos == 0) continue;
                uint8_t last = player_pos;
                player_pos--;
                update_paddle_pos(last, player_pos, 1);
            } else if(key_event.key == KEY_ArrowDown) {
                if(player_pos == 1080 - (paddle_height / 2)) continue;
                uint8_t last = player_pos;
                player_pos++;
                update_paddle_pos(last, player_pos, 0);
            } else if(key_event.key == KEY_D) {
                debug = !debug;
            }

            iteration = 0;
            input_locked = true;
        }

        if(paused) continue;

        if(tick % 10) {
            vbe_fillrect(1920, 0, 1920 - 20, 20, 0x00ff00ff);
            simulate_ball();
            simulate_bot();
        } else {
            vbe_fillrect(1920, 0, 1920 - 20, 20, 0x0000ff00);
        }
    }
}

void Pong::update_paddle_pos(uint8_t last_pos, uint8_t new_pos, uint8_t side) {
    if(debug) {
        clear(game_buffer, 0x00ff00ff);
    }

    if(side == 0) {
        fill_rect(game_buffer, uRect32(10, last_pos - (paddle_height / 2), 10 + paddle_width, last_pos + (paddle_height / 2)), 0x00000000);
        fill_rect(game_buffer, uRect32(10, new_pos - (paddle_height / 2), 10 + paddle_width, new_pos + (paddle_height / 2)), 0x00ffffff);

        // Probably a more efficient way of doing it
        vbe_render_part(game_buffer, uPoint32(10, 20), uRect32(10, 20, 10 + 25, 1080 - 40));
    } else if(side == 1) {
        fill_rect(game_buffer, uRect32(1920 - 35, last_pos - (paddle_height / 2), (1920 - 35) + paddle_width, last_pos + (paddle_height / 2)), 0x00000000);
        fill_rect(game_buffer, uRect32(1920 - 35, new_pos - (paddle_height / 2), (1920 - 35) + paddle_width, new_pos + (paddle_height / 2)), 0x00ffffff);

        // Probably a more efficient way of doing it
        vbe_render_part(game_buffer, uPoint32(1920 - 45, 20), uRect32((1920 - 35), 20, (1920 - 35) + 25, 1080 - 40));
    }
}

void Pong::update_ball_pos(uPoint32 last_pos, uPoint32 new_pos) {
    //clear(game_buffer, 0x00ff00ff); // Test

    draw_filled_circle(game_buffer, uCircle32(last_pos.x, last_pos.y, circle_radius), 0x00000000);
    draw_filled_circle(game_buffer, uCircle32(new_pos.x, new_pos.y, circle_radius / 2), 0x00ffffff);

    //fill_rect(game_buffer, uRect32(new_pos.x, new_pos.y, circle_radius, circle_radius), 0x00ffffff);
    //draw_filled_circle(game_buffer, uCircle32(new_pos.x, new_pos.y, circle_radius / 2), 0x000000ff);

    //GUI::vbe_render_part(game_buffer, uPoint32(last_pos.x, last_pos.y), uRect32(last_pos.x, last_pos.y, circle_radius * 2, circle_radius * 2));
    //GUI::vbe_render_part(game_buffer, uPoint32(new_pos.x, new_pos.y), uRect32(new_pos.x, new_pos.y, circle_radius * 2, circle_radius * 2));
    //GUI::vbe_render_part(game_buffer, uPoint32(new_pos.x, new_pos.y), uRect32(new_pos.x, new_pos.y, circle_radius * 2, circle_radius * 2));

    //GUI::vbe_render_part(game_buffer, last_pos, GUI::uRect32(last_pos.x, last_pos.y, circle_radius * 2, circle_radius * 2));
	//GUI::vbe_render_part(game_buffer, new_pos, GUI::uRect32(new_pos.x, new_pos.y, circle_radius * 2, circle_radius * 2));
    //vbe_render_part(game_buffer, uPoint32(0, 0), uRect32(new_pos.x, new_pos.y, 150, 150));
    //vbe_render_all(game_buffer, uPoint32(0, 0));

    // Can't work it out, so I'll render the whole thing
    vbe_render_part(game_buffer, uPoint32(45, 20), uRect32(35, 20, 1920 - (35 * 2), 1080 - 40));
}

/*void Pong::update_ball_pos(GUI::uPoint32 last_pos, GUI::uPoint32 new_pos) {
	int radius = circle_radius;
    int size = radius * 2;
    int margin = 1; // Give some padding from (0,0)

    GUI::uPoint32 draw_origin = GUI::uPoint32(margin, margin);
    GUI::uCircle32 circle(draw_origin.x + radius, draw_origin.y + radius, radius);

    // Clear and draw
    GUI::rect(game_buffer, GUI::uRect32(0, 0, size, size), 0x00000000);
    draw_filled_circle(game_buffer, circle, 0x00ffffff);

    // Now blit the slightly bigger region
    GUI::vbe_render_part(game_buffer, new_pos, GUI::uRect32(0, 0, size + margin * 4, size + margin * 4));
}*/

void Pong::simulate_ball() {
    //float radians = ball_direction * M_PI / 180.0f;
    uint32_t radians = ball_direction * M_PI / 180.0f;

    // Velocity
    //float dx = cos(radians) * ball_speed;
	//float dy = sin(radians) * ball_speed;
    uint32_t dx = cos(radians) * ball_speed;
    uint32_t dy = cos(radians) * ball_speed;

    // New position
    //uint32_t new_x = ball.x + static_cast<int>(dx);
	//uint32_t new_y = ball.y + static_cast<int>(dy);
    uint32_t new_x = ball.x + dx;
	uint32_t new_y = ball.y + dy;

    // Convert it to a point
    uPoint32 new_pos = uPoint32(new_x, new_y);

    update_ball_pos(ball, new_pos);
}

void Pong::simulate_bot() {
    uint8_t last = bot_pos;
    bot_pos = ball.y;
    update_paddle_pos(last, bot_pos, 0);
}

void Pong::draw_main() {
    GUI::fill_rect(game_buffer, uRect32(0, 0, 1920, 20), 0x00ffffff); // Top bar
    GUI::fill_rect(game_buffer, uRect32(0, 1920 - 20, 1920, 20), 0x00ffffff); // Bottom bar

    GUI::vbe_render_part(game_buffer, uPoint32(0, 0), uRect32(0, 0, 1920, 20));
    GUI::vbe_render_part(game_buffer, uPoint32(0, 1080 - 20), uRect32(0, 0, 1920, 20));

    // Temporary
    print_string("ChoacuryOS Recover Mode: Secret pong game", 3, 3, 0x00000000);
    print_string("Esc=Exit  Up/Down=Move  Space=Start/Stop  D=Debug  R=Reset", 3, 1080 - 17, 0x00000000);
}

void Pong::draw_about() {
    uint32_t background_colour = 0x00111111;
    uint32_t text_colour = 0x00ffffff;
    uint32_t box_colour = 0x00000000;
    uint32_t border_colour = 0x00ffffff;

    clear(game_buffer, background_colour);

    uint32_t width = 625;
    uint32_t height = 150;
    uint32_t x = (1920 / 2) - (width / 2);
    uint32_t y = (1080 / 2) - (height / 2);
    uint32_t bottom = y + height;

    fill_rect(game_buffer, uRect32(x, y, width, height), box_colour);
    rect(game_buffer, uRect32(x, y, width, height), border_colour);

    vbe_render_all(game_buffer, uPoint32(0, 0));

    // Temporary
    print_string("This is a secret pong game - Made by MrBisquit (WTDawson)", x + 10, y + 10, text_colour);
    print_string("It's pretty simple to play, but may be a bit buggy. Just as a heads up, there", x + 10, y + 10 + (13 * 1), text_colour);
    print_string("is only a small chance that you will win. The bot is good.", x + 10, y + 10 + (13 * 2), text_colour);

    print_string("How to play:", x + 10, bottom - 10 - (13 * 3), text_colour);
    print_string("Up/Down arrow keys to move your paddle (on the right hand side). R to reset scores.", x + 10, bottom - 10 - (13 * 2), text_colour);
    print_string("Space to start/stop the game, and D to enter debug mode (not recommended).", x + 10, bottom - 10 - 13, text_colour);
}