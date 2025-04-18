// This is terrible, I know, but we can easily (ish) add options
// If you create anything new, like drivers, add a testing feature here, or gives some information
//
// There are a few issues with this though, like for some reason (even though I've tried to fix it)
// it moves up/down twice.
//
// Created by MrBisquit (WTDawson)
// Some features may not be implemented
// (Hidden pong game by pressing 'p' on the main menu, barely works though)

#include "recovery.h"
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
#include "pong.hpp"

struct Option {
    Option(char* name, uint8_t position, bool selected) : name(name), position(position), selected(selected) { }
public:
    char* name;
    uint8_t position;
    bool selected;
};

static bool keyboard = false; // Set to true when a key is pressed, wait a while and then disable it to protect keyboard input
static uint8_t tick = 0;
static uint8_t option_mode = 0;

void draw_option(GUI::Buffer buffer, Option option, bool selected) {
    if(option.selected == true || selected) {
        //GUI::fill_rect(buffer, GUI::uRect32(0, 20 + (option.position * 20), 1920, 20), 0x00aaaaaa);
        GUI::fill_rect(buffer, GUI::uRect32(0, 0, 1920, 1080), 0x00aaaaaa); // Temporary
    }
    else {
        //GUI::fill_rect(buffer, GUI::uRect32(0, 20 + (option.position * 20), 1920, 20), 0x00000000);
        GUI::fill_rect(buffer, GUI::uRect32(0, 0, 1920, 1080), 0x00000000); // Temporary
    }

    uint32_t y = 20 + (option.position * 20);
    GUI::vbe_render_part(buffer, GUI::uPoint32(0, y), GUI::uRect32(0, y, 1920, 20));

    // Temporary
    print_string((const char*)option.name, 5, 20 + 5 + (option.position * 20), 0x00ffffff);
}

void draw_main(GUI::Buffer buffer) {
    // Draw the main screen
    GUI::clear(buffer, 0x00000000); // Clear it
    GUI::vbe_render_all(buffer, GUI::uPoint32(0, 0)); // Render it

    // Draw the logo
    GUI::fill_rect(buffer, GUI::uRect32(0, 0, 1920, 20), 0x00aaaaaa); // Title bar
    GUI::fill_rect(buffer, GUI::uRect32(0, 1080 - 20, 1920, 20), 0x00aaaaaa); // Bottom help bar

    //GUI::vbe_render_all(buffer, GUI::uPoint32(0, 0)); // Render it

    GUI::vbe_render_part(buffer, GUI::uPoint32(0, 0), GUI::uRect32(0, 0, 1920, 20)); // Title bar
    GUI::vbe_render_part(buffer, GUI::uPoint32(0, 1080 - 20), GUI::uRect32(0, 1080 - 20, 1920, 20)); // Bottom help bar

    // Temporary text, this needs to go above when fixed
    print_string((const char*)"ChoacuryOS Recovery Mode", 5, 3, 0x00000000);
    if(option_mode == 0) {
        print_string((const char*)"ESC=Exit  Up/Down=Navigate  Return=Enter  Space=Select", 5, 1080 -  17, 0x00000000);
    } else if(option_mode == 1) {
        print_string((const char*)"ESC=Exit", 5, 1080 -  17, 0x00000000);
    }
}

void test_menu(GUI::Buffer buffer) {
    //draw_main(buffer);

    uint8_t selected_option = -1;
    bool need_redraw = true;

    draw_option(buffer, Option((char*)"System tests", 0, false), false); // Draw the first option

    for(;;) {
        tick++;

        key_event_t key_event;
        ps2_get_key_event(&key_event);

        if(keyboard == true && tick == 3) {
            keyboard = false;
            tick = 0;
        }
        if(keyboard == true) continue;

        if(key_event.key == KEY_Escape) {
            break;
        }

        if(key_event.key == KEY_ArrowUp) {
            if(selected_option == 0) continue;
            selected_option--;
            need_redraw = true;
        }

        if(key_event.key == KEY_ArrowDown) {
            if(selected_option == 3) continue;
            selected_option++;
            need_redraw = true;
        }

        if(key_event.key == KEY_Enter) {
            if(selected_option == 0) {
                break;
            } else if(selected_option == 1) {
                GUI::clear(buffer, 0x00000000); // Clear it
                option_mode = 1;
                draw_main(buffer);

                GUI::rect(buffer, GUI::uRect32(32, 32, 100, 100), 0x00ff0000);
                GUI::fill_rect(buffer, GUI::uRect32(32 + 100 + 10, 32, 100, 100), 0x00ff0000);

                GUI::draw_circle(buffer, GUI::uCircle32(32, 32 + 100 + 10, 50), 0x00ff0000);
                GUI::draw_filled_circle(buffer, GUI::uCircle32(32 + 100 + 10, 32 + 100 + 10, 50), 0x00ff0000);

                GUI::vbe_render_part(buffer, GUI::uPoint32(0, 20), GUI::uRect32(32, 32, 1920, 1080 - 40)); // Render it
                
                uint32_t i = 0;
                uint8_t mode = 0;

                for(;;) {
                    i++;
                    if(i > 1000) i = 0;

                    uint32_t colour = 0x00000000 + (0x101010 * i);
                    if(i > 250) colour = 0x00ff0000;
                    if(i > 500) colour = 0x0000ff00;
                    if(i > 750) colour = 0x000000ff;

                    if(mode == 0) {
                        for (uint32_t y = 0; y < 100; y++)
                        {
                            for (uint32_t x = 0; x < 100; x++)
                            {
                                GUI::put_pixel(buffer, GUI::uPoint32(32 + x, 32 + 200 + 20 + y), colour + (0x101010 * (i + x + y)));
                            }
                        }

                        GUI::rect(buffer, GUI::uRect32(32, 32, 100, 100), colour);
                        GUI::fill_rect(buffer, GUI::uRect32(32 + 100 + 10, 32, 100, 100), colour);

                        GUI::draw_circle(buffer, GUI::uCircle32(32, 32 + 100 + 10, 50), colour);
                        GUI::draw_filled_circle(buffer, GUI::uCircle32(32 + 100 + 10, 32 + 100 + 10, 50), colour);
                    } else if(mode == 1) {
                        for (uint32_t y = 0; y < 1080 - 40; y++)
                        {
                            for (uint32_t x = 0; x < 1920; x++)
                            {
                                GUI::put_pixel(buffer, GUI::uPoint32(x, y), colour + (0x00101010 + (x + y * i)));
                            }
                        }
                    }

                    GUI::vbe_render_part(buffer, GUI::uPoint32(0, 20), GUI::uRect32(32, 32, 1920, 1080 - 60));

                    print_string((const char*)"Left/Right to cycle between modes", 5, 1080 - 40, 0x00ffffff);

                    tick++;

                    key_event_t key_event;
                    ps2_get_key_event(&key_event);

                    if(keyboard == true && tick >= 3) {
                        keyboard = false;
                        tick = 0;
                    }
                    if(keyboard == true) continue;

                    if(key_event.key == KEY_E || key_event.key == KEY_Escape) {
                        GUI::clear(buffer, 0x00000000); // Clear it
                        option_mode = 0;
                        draw_main(buffer);
                        break;
                    } else if(key_event.key == KEY_ArrowLeft) {
                        if(mode == 0) continue;
                        mode--;
                        GUI::fill_rect(buffer, GUI::uRect32(32, 32, 1920, 1080 - 60), 0x00000000);
                    } else if(key_event.key == KEY_ArrowRight) {
                        if(mode == 1) continue;
                        mode++;
                        GUI::fill_rect(buffer, GUI::uRect32(32, 32, 1920, 1080 - 60), 0x00000000);
                    }
                }

                need_redraw = true;
            } else if(selected_option == 2) {
                GUI::clear(buffer, 0x00000000); // Clear it
                GUI::vbe_render_all(buffer, GUI::uPoint32(0, 0));
                option_mode = 1;
                //draw_main(buffer);

                // Not fully implemented

                draw_option(buffer, Option("Colour Spectrum Test", 0, true), true);
                draw_option(buffer, Option("Waiting...", 1, false), false);

                draw_option(buffer, Option("Basic RGB Test", 2, true), true);
                draw_option(buffer, Option("Waiting...", 3, false), false);

                draw_option(buffer, Option("Static Test", 4, true), true);
                draw_option(buffer, Option("Waiting...", 5, false), false);

                // Actually run the tests
                draw_option(buffer, Option("Running...", 1, false), false);
                // Show all ARGB colours
                draw_option(buffer, Option("Passed", 1, false), false);

                draw_option(buffer, Option("Running...", 3, false), false);
                vbe_fillrect(1920 / 2, 0, 1920, 1080, 0x00ff0000);
                vbe_fillrect(1920 / 2, 0, 1920, 1080, 0x0000ff00);
                vbe_fillrect(1920 / 2, 0, 1920, 1080, 0x000000ff);

                draw_option(buffer, Option("These tests (especially the first one) tend to take absolutely ages, and it'll go through them one by one.", 15, false), false);
                draw_option(buffer, Option("At the end, you'll be able to see the results and press Escape to exit.", 16, false), false);
                draw_option(buffer, Option("You may notice some text randomly on the left hand side, you can ignore that as it's just debug output from the tests.", 18, false), false);
                draw_option(buffer, Option("Warning: It may flash colours very quickly", 20, true), true);

                uint32_t colour = 0x00000000;
                for (uint8_t r = 0; r < 15; r++)
                {
                    colour += 0x00110000;
                    vbe_fillrect(1920 / 2, 0, 1920, 1080, colour);
                    for (uint8_t g = 0; g < 15; g++)
                    {
                        colour += 0x00001100;
                        vbe_fillrect(1920 / 2, 0, 1920, 1080, colour);
                        for (uint8_t b = 0; b < 15; b++)
                        {
                            colour += 0x00000011;
                            vbe_fillrect(1920 / 2, 0, 1920, 1080, colour);

                            print_string((const char*)(b * (256 / 15)), 3, 174, b * 0x00000011);

                            vbe_fillrect(3, 186, 1920 / 2, 198, 0x00000000);
                            print_string((const char*)((r * b * g) * (256 / 15)), 3, 186, 0x00ffffff);
                        }
                        print_string((const char*)(g * (256 / 15)), 3, 162, g * 0x00001100);
                        vbe_fillrect(3, 174, 1920 / 2, 186, 0x00000000);
                    }
                    print_string((const char*)(r * (256 / 15)), 3, 150, 0x00ffffff);
                    vbe_fillrect(3, 162, 1920 / 2, 174, r * 0x00110000);
                }
                
                draw_option(buffer, Option("Passed", 1, false), false);
                draw_option(buffer, Option("Running...", 3, false), false);
                vbe_fillrect(1920 / 2, 0, 1920, 1080, 0x00000000);

                for(;;) {
                    key_event_t key_event;
                    ps2_get_key_event(&key_event);

                    if(key_event.key == KEY_Escape) break;
                }
            } else if(selected_option == 3) {
                uint32_t background_colour = 0x00111111;
                uint32_t text_colour = 0x00ffffff;
                uint32_t box_colour = 0x00000000;
                uint32_t border_colour = 0x00ffffff;

                GUI::clear(buffer, background_colour);

                uint32_t width = 700;
                uint32_t height = 150;
                uint32_t x = (1920 / 2) - (width / 2);
                uint32_t y = (1080 / 2) - (height / 2);
                uint32_t bottom = y + height;

                GUI::fill_rect(buffer, GUI::uRect32(x, y, width, height), box_colour);
                GUI::rect(buffer, GUI::uRect32(x, y, width, height), border_colour);

                GUI::vbe_render_all(buffer, GUI::uPoint32(0, 0));

                // Temporary
                print_string("This allows you to test the different colours (Red, Green, Blue, Black, and White).", x + 10, y + 10, text_colour);
                print_string("1=Red, 2=Green, 3=Blue, 4=White, 5=Black", x + 10, y + 10 + (13 * 1), text_colour);
                print_string("C=Cycle (Cycles through the different colours)", x + 10, y + 10 + (13 * 2), text_colour);
                print_string("To go back, press escape", x + 10, y + 10 + (13 * 3), text_colour);
                print_string("(This menu will dissapear after you press a key)", x + 10, y + 10 + (13 * 4), text_colour);

                print_string("ChoacuryOS Recovery Mode", x + 10, bottom - 10 - (13 * 3), text_colour);
                print_string("System tests", x + 10, bottom - 10 - (13 * 2), text_colour);
                print_string("Fullscreen RGB test", x + 10, bottom - 10 - 13, text_colour);

                for (;;) {
                    key_event_t key_event;
                    ps2_get_key_event(&key_event);

                    if(key_event.key == KEY_Escape) break;
                    else if(key_event.key == KEY_1) {
                        GUI::clear(buffer, 0x00ff0000);
                        GUI::vbe_render_all(buffer, GUI::uPoint32(0, 0));
                    } else if(key_event.key == KEY_2) {
                        GUI::clear(buffer, 0x0000ff00);
                        GUI::vbe_render_all(buffer, GUI::uPoint32(0, 0));
                    } else if(key_event.key == KEY_3) {
                        GUI::clear(buffer, 0x000000ff);
                        GUI::vbe_render_all(buffer, GUI::uPoint32(0, 0));
                    } else if(key_event.key == KEY_4) {
                        GUI::clear(buffer, 0x00ffffff);
                        GUI::vbe_render_all(buffer, GUI::uPoint32(0, 0));
                    } else if(key_event.key == KEY_5) {
                        GUI::clear(buffer, 0x00000000);
                        GUI::vbe_render_all(buffer, GUI::uPoint32(0, 0));
                    }
                }
            }
        }

        if(need_redraw) {
            draw_option(buffer, Option((char*)"Back", 1, selected_option == 0), selected_option == 0);
            draw_option(buffer, Option((char*)"GUI tests", 2, selected_option == 1), selected_option == 1);
            draw_option(buffer, Option((char*)"VBE tests", 3, selected_option == 2), selected_option == 2);
            draw_option(buffer, Option((char*)"Fullscreen RGB test", 4, selected_option == 3), selected_option == 3);
        
            need_redraw = false;
        }
    }
}

// cpuid and get_cpu_info ripped from shell.c
void cpuid(uint32_t eax_in, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx) {
    __asm__ volatile (
        "cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "a" (eax_in)
    );
}

void get_cpu_info(char* vendor, char* brand) {
    uint32_t eax, ebx, ecx, edx;

    /* fetch the cpu vendor */
    cpuid(0, &eax, &ebx, &ecx, &edx);
    ((uint32_t*)vendor)[0] = ebx;
    ((uint32_t*)vendor)[1] = edx;
    ((uint32_t*)vendor)[2] = ecx;
    vendor[12] = '\0';

    /* fetch the cpu brand, if supported */
    cpuid(0x80000000, &eax, &ebx, &ecx, &edx);
    if (eax >= 0x80000004) {
        uint32_t* brand_ptr = (uint32_t*)brand;
        cpuid(0x80000002, &brand_ptr[0], &brand_ptr[1], &brand_ptr[2], &brand_ptr[3]);
        cpuid(0x80000003, &brand_ptr[4], &brand_ptr[5], &brand_ptr[6], &brand_ptr[7]);
        cpuid(0x80000004, &brand_ptr[8], &brand_ptr[9], &brand_ptr[10], &brand_ptr[11]);
        brand[48] = '\0';
    } else {
        brand[0] = '\0';
    }
}

void sys_info(GUI::Buffer buffer) {
    GUI::clear(buffer, 0x00000000); // Clear it
    option_mode = 1;
    draw_main(buffer);

    char mem_mib_buffer[20];
    uint64_to_string(g_total_pmm_bytes / 1024 / 1024, mem_mib_buffer);
    mem_mib_buffer[19] = 0;                                 // <-- to prevent undefined behaviour
    char cpu_vendor[13];
    char cpu_brand[49];

    get_cpu_info(cpu_vendor, cpu_brand);

    draw_option(buffer, Option("ChoacuryOS System Information", 0, true), true);

    draw_option(buffer, Option("BUILD", 2, true), true);
    draw_option(buffer, Option(__DATE__ " @ " __TIME__, 3, false), false);

    draw_option(buffer, Option("KERNAL", 4, true), true);
    draw_option(buffer, Option("Choacury Standard", 5, false), false);

    draw_option(buffer, Option("SHELL", 6, true), true);
    draw_option(buffer, Option("chsh-0.0.0.0041e-dev", 7, false), false);

    draw_option(buffer, Option("RAM (MiB)", 8, true), true);
    draw_option(buffer, Option(mem_mib_buffer, 9, false), false);

    draw_option(buffer, Option("CPU VENDOR", 10, true), true);
    draw_option(buffer, Option(cpu_vendor, 11, false), false);

    draw_option(buffer, Option("CPU BRAND", 12, true), true);
    draw_option(buffer, Option((char*)(cpu_brand[0] ? cpu_brand : "Unknown"), 13, false), false);

    draw_option(buffer, Option("MADE BY", 14, true), true);
    draw_option(buffer, Option("Team Choacury", 15, false), false);
    //draw_option(buffer, Option("Pineconium, MrBisquit (WTDawson), Bananymous, Keiran-brooks, vuk5002 (VuleSuma)", 16, false), false);
    draw_option(buffer, Option("   Pineconium", 16, false), false);
    draw_option(buffer, Option("         (Head Dev) Kernel, Software, and GUI developer", 17, false), false);
    draw_option(buffer, Option("   MrBisquit (WTDawson)", 18, false), false);
    draw_option(buffer, Option("         (Dev) Kernel, Software, GUI, and website developer", 19, false), false);
    draw_option(buffer, Option("   Bananymous (Oskari Alaranta)", 20, false), false);
    draw_option(buffer, Option("         (Dev) Kernel, and Software developer", 21, false), false);
    draw_option(buffer, Option("   Keiran-brooks (Keiran)", 22, false), false);
    draw_option(buffer, Option("         (Dev) Kernel, Software, GUI, and website developer", 23, false), false);
    draw_option(buffer, Option("   vuk5002 (VuleSuma) - Our drivers guy", 24, false), false);
    draw_option(buffer, Option("         (Dev) Kernel, Software, and GUI developer", 25, false), false);

    draw_option(buffer, Option("For more information:", 27, false), false);
    draw_option(buffer, Option("   Documentation:     https://teamchoacury.github.io/docs/", 28, false), false);
    draw_option(buffer, Option("   GitHub repository: https://github.com/Pineconium/ChoacuryOS/", 29, false), false);

    draw_option(buffer, Option("ChoacuryOS", 31, true), true);
    draw_option(buffer, Option("ChoacuryOS is a custom-built OS written in C, C++, and Assembly. This is an open-source project, meaning you can contribute! You may also modify ChoacuryOS to your liking if you want to make your own operating system. There are many tools", 32, false), false);
    draw_option(buffer, Option("that have been built to make it easier to create things for the operating system, e.g. RIMTils, which is software designed to run on Windows, which allows you to convert to and from Bitmaps and RIMs, among other things.", 33, false), false);
    draw_option(buffer, Option("This recovery mode comes as part of the operating system, as well as the fallback window manager (FBWM). However, you may have LNDE installed, which is the default desktop environment for ChoacuryOS, as well as many other tools built by", 35, false), false);
    draw_option(buffer, Option("our wonderful developers (as mentioned above, if your name is not on there, please contact us).", 36, false), false);

    draw_option(buffer, Option("About ChoacuryOS Recovery Mode", 38, true), true);
    draw_option(buffer, Option("This was built by MrBisquit (WTDawson) as a way to test certain components, including the GUI, and get access to the fallback window manager. This mode will include tools that will help you recover ChoacuryOS and resolve any issues with", 39, false), false);
    draw_option(buffer, Option("it.", 40, false), false);

    for(;;) {
        tick++;

        key_event_t key_event;
        ps2_get_key_event(&key_event);

        if(keyboard == true && tick == 3) {
            keyboard = false;
            tick = 0;
        }
        if(keyboard == true) continue;

        if(key_event.key == KEY_Escape) {
            //GUI::clear(buffer, 0x00000000); // Clear it
            //GUI::vbe_render_all(buffer, GUI::uPoint32(0, 0)); // Render it
            break;
        }
    }
}

void main_menu(GUI::Buffer buffer) {
    GUI::clear(buffer, 0x00000000); // Clear it
    draw_main(buffer);

    //draw_option(buffer, Option((char*)"System tests", 0, true)); // Draw the first option
    //draw_option(buffer, Option((char*)"FB WM & DE", 1, true)); // Draw the first option
    //draw_option(buffer, Option((char*)"System information", 2, false)); // Draw the second option

    uint8_t selected_option = -1;
    bool need_redraw = true;

    uint8_t state = 0;

    for(;;) {
        tick++;

        key_event_t key_event;
        ps2_get_key_event(&key_event);

        if(keyboard == true && tick == 3) {
            keyboard = false;
            tick = 0;
        }
        if(keyboard == true) continue;

        if(key_event.key == KEY_Escape) {
            break;
        } else if(key_event.key == KEY_ArrowUp) {
            if(selected_option == 0) continue;
            selected_option--;
            need_redraw = true;
            keyboard = true;
            tick = 0;
            continue;
        } else if(key_event.key == KEY_ArrowDown) {
            if(selected_option == 2) continue;
            selected_option++;
            need_redraw = true;
            keyboard = true;
            tick = 0;
            continue;
        } else if(key_event.key == KEY_Enter) {
            //GUI::clear(buffer, 0x00000000); // Clear it
            need_redraw = true;

            if(selected_option == 0) {
                test_menu(buffer);
            } else if(selected_option == 1) {
                start_desktop();
            } else if(selected_option == 2) {
                sys_info(buffer);
            }

            draw_main(buffer);
            selected_option = -1;
            need_redraw = true;
            keyboard = true;
            tick = 0;

            continue;
        } else if(key_event.key == KEY_P) {
            // Hidden pong game
            Pong pong = Pong(buffer);
            pong.init_pong();
        }

        state++;
        if(state <= 100) {
            vbe_fillrect(32, 32 + 300, 32 + 300, 32 + 600, 0x00ff0000);
        } else if(state <= 200) {
            vbe_fillrect(32, 32 + 300, 32 + 300, 32 + 600, 0x0000ff00);
        } else if(state > 200) {
            vbe_fillrect(32, 32 + 300, 32 + 300, 32 + 600, 0x000000ff);
        }

        if(state >= 400) state = 0;

        if(need_redraw) {
            draw_option(buffer, Option((char*)"System tests", 0, selected_option == 0), selected_option == 0); // Draw the first option
            draw_option(buffer, Option((char*)"FB WM & DE", 1, selected_option == 1), selected_option == 1); // Draw the first option
            draw_option(buffer, Option((char*)"System information", 2, selected_option == 2), selected_option == 2); // Draw the second option
        
            need_redraw = false;
        }
    }
}

extern "C" void start_recovery() {
    print_string((const char*)"Entering recovery mode...", 5, 3, 0x00000000);

    //vbe_clear_screen(0x000000ff);
    //vbe_clear_screen(0xffffffff);

    GUI::Buffer buffer = GUI::Buffer((uint32_t*)kmalloc(1920 * 108 * sizeof(uint32_t)), 1920, 1080);
    /*GUI::clear(buffer, 0x000000ff); // Clear it
    GUI::vbe_render_all(buffer, GUI::uPoint32(0, 0)); // Render it

    //GUI::draw_line(buffer, GUI::uPoint32(0, 0), GUI::uPoint32(1920, 100), 0x00ff0000); // Draw a line

    //GUI::fill_rect(buffer, GUI::uRect32(0, 0, 1920, 1080), 0x00ff0000); // Draw a filled rectangle

    GUI::PSF1_FONT default_font;
    GUI::Utils::data_to_psf1(&default_font, GUI::unifont);

    GUI::draw_text(buffer, GUI::uText((char*)"Test", 5, 5, &default_font, 0x00ffffff)); // Draw some text
    print_string((const char*)"Test", 5, 35, 0x00ffffff); // Draw some text

    //GUI::fill_rect(buffer, GUI::uRect32(32, 32, 500, 100), 0x00ff0000); // Placeholder for logo
    GUI::fill_rect(buffer, GUI::uRect32(0, 0, 1920, 20), 0x00ffffff); // Placeholder for logo
    GUI::fill_rect(buffer, GUI::uRect32(0, 1080 - 20, 1920, 20), 0x00ffffff); // Bottom help bar

    GUI::vbe_render_all(buffer, GUI::uPoint32(0, 0)); // Render it

    // Options area
    //GUI::fill_rect(buffer, GUI::uRect32(32, 140 + 32, 1000, 400), 0x00000000); // Options area
    //GUI::fill_rect(buffer, GUI::uRect32(32, 140 + 32, 1000, 20), 0x00aaaaaa); // Option 1 selected

    GUI::fill_rect(buffer, GUI::uRect32(0, 20, 1920, 1080 - 40), 0x00000000); // Options area
    GUI::fill_rect(buffer, GUI::uRect32(0, 20, 1920, 20), 0x00aaaaaa); // Option 1 selected

    GUI::vbe_render_all(buffer, GUI::uPoint32(0, 0)); // Render it
    //GUI::vbe_render_part(buffer, GUI::uRect32(0, 0, 100, 100)); // Render it

    //print_string((const char*)"ChoacuryOS Recovery Mode", 32, 140, 0x00ffffff);
    print_string((const char*)"ChoacuryOS Recovery Mode", 5, 3, 0x00000000);
    print_string((const char*)"ESC=Exit  Up/Down=Navigate  Return=Enter  Space=Select", 5, 1080 -  17, 0x00000000);
    //print_string((const char*)"Test GUI", 35, 140 + 32 + 2, 0x00ffffff);
    //print_string((const char*)"System information", 35, 140 + 32 + 2 + 20, 0x00ffffff);
    print_string((const char*)"Test GUI", 5, 20 + 2, 0x00ffffff);
    print_string((const char*)"System information", 5, 20 + 2 + 20, 0x00ffffff);*/

    //vbe_clear_screen(0x000000ff); // Clear the screen with a blue colour

    GUI::clear(buffer, 0x00000000); // Clear it
    //draw_main(buffer);

    main_menu(buffer);

    for(;;) {}
}