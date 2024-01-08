#include "vga.h"

/* 
VGA Stuff, aka. (Extremely Limited) Graphics.

Currently this will only work with Virtual Machines for some reason.
*/

/* Function to clear off the screen */
void k_clear_screen()
{
    char *vidmem = (char *) 0xb8000;
    unsigned int i=0;
    while(i < (80*25*2))
    {
        vidmem[i]=' ';
        i++;
        vidmem[i]=TC_WHITE;
        i++;
    };
};

/* A little function to print text 
Without it, you'll just see a black screen*/
unsigned int k_printf(const char *message, unsigned int line, unsigned int tcolour)
{
    char *vidmem = (char *) 0xb8000;
    unsigned int i=0;

    i=(line*80*2);

    while(*message!=0)
    {
        if(*message=='\n')               // <-- Check for a new line
        {
            line++;
            i=(line*80*2);
            message++;
        } else {
            vidmem[i]=*message;
            message++;
            i++;
            vidmem[i]=tcolour;          // <-- Text Colour Controller.
                                        // E.g. if you want it lime (TC_LIME), you do k_printf("string", 0, 10) as 10 as A in hexadecimal (0x0A)
            i++;
        };
    };

    return 0;
};