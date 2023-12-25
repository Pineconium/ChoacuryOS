/* 
VGA Stuff, aka. (Extremely Limited) Graphics.

Currently this will only work with Virtual Machines for some reason.
*/

/* Colour Stuff */
/* All of them are rendered on a black background (0x0~) */
#define TC_BLUE 0x01
#define TC_GREEN 0x02
#define TC_CYAN 0x03
#define TC_DKRED 0x04
#define TC_MANGT 0x05
#define TC_BROWN 0x06       // <-- More of a darker yellow then brown tbh.
#define TC_WHITE 0x07       // <-- Renders Grey/White Text (Primary Text Style)
#define TC_DGREY 0x08
#define TC_LBLUE 0x09
#define TC_LIME 0x0A
#define TC_LCYAN 0x0B
#define TC_LRED 0x0C
#define TC_PINK 0x0D
#define TC_YELLO 0x0E
#define TC_BRIGHT 0x0F      // <-- This makes it actually white and not light grey as used in TC_WHITE


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
unsigned int k_printf(char *message, unsigned int line, unsigned int tcolour)
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
            *message++;
        } else {
            vidmem[i]=*message;
            *message++;
            i++;
            vidmem[i]=tcolour;          // <-- Text Colour Controller.
                                        // E.g. if you want it lime (TC_LIME), you do k_printf("string", 0, 10) as 10 as A in hexadecimal (0x0A)
            i++;
        };
    };
};