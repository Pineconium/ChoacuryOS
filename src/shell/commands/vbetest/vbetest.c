#include "vbetest.h"

#include "../../shell.h"
#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"

int shell_vbetest_command(int argc, const char** argv) {
    //BgaSetVideoMode(FHD,VBE_DISPI_BPP_32,1,1);
    //BgaSetVideoMode(1024, 768, 32, 1, 1)
    BgaSetVideoMode(1920, 1080, 32, 1, 1);
    vbe_putpixel(0,0, 0xffffff);
    int width = 1920;
    int height = 1080;
    for (size_t x = 0; x < width; x++)
    {
        for (size_t y = 0; y < height; y++)
        {
            uint8_t red = (x * 255) / width;
            uint8_t green = (y * 255) / height;
            uint8_t blue = ((x + y) * 255) / (width + height);

            uint32_t color = (red << 16) | (green << 8) | blue;

            vga_putpixel(x, y, color);
        }
    }

    for (size_t x = 0; x < 1024; x++)
    {
        for (size_t y = 0; y < 768; y++)
        {
            vga_putpixel(x, y, 0xffffff);
            vbe_putpixel(x, y, 0xffffff);
        }
    }
    
    return 0;
}