/* now we can have better graphics */

#include "vbe.h"
#include "types.h"

/* move this somewhere else */
static inline void outpw(u16 port, u16 value) {
    __asm__ __volatile__ ("outw %0, %1" : : "a"(value), "Nd"(port));
}

static inline u16 inpw(u16 port) {
    u16 value;
    __asm__ __volatile__ ("inw %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}
// End

// DONT TRY THIS IN BOCHS! BOCHS IGNORES BANK ADDRESS WRITES WHEN
// LFB IS ACTIVE
#define BANK_ADDRESS 0xA0000
void vbe_putpixel(u32 x, u32 y, u32 color) {
	u32 *framebuffer = (u32*)BANK_ADDRESS;
    framebuffer[y * 1920 /*todo: don't hardcode*/ + x] = color;
}


void BgaWriteRegister(unsigned short IndexValue, unsigned short DataValue){
    outpw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    outpw(VBE_DISPI_IOPORT_DATA, DataValue);
}

unsigned short BgaReadRegister(unsigned short IndexValue){
    outpw(VBE_DISPI_IOPORT_INDEX, IndexValue);
    return inpw(VBE_DISPI_IOPORT_DATA);
}

int BgaIsAvailable(void){
    return (BgaReadRegister(VBE_DISPI_INDEX_ID) == VBE_DISPI_ID4);
}

void BgaSetVideoMode(unsigned int Width, unsigned int Height, unsigned int BitDepth, int UseLinearFrameBuffer, int ClearVideoMemory){
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    BgaWriteRegister(VBE_DISPI_INDEX_XRES, Width);
    BgaWriteRegister(VBE_DISPI_INDEX_YRES, Height);
    BgaWriteRegister(VBE_DISPI_INDEX_BPP, BitDepth);
    BgaWriteRegister(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_ENABLED |
        (UseLinearFrameBuffer ? VBE_DISPI_LFB_ENABLED : 0) |
        (ClearVideoMemory ? 0 : VBE_DISPI_NOCLEARMEM));
}

void BgaSetBank(unsigned short BankNumber){
    BgaWriteRegister(VBE_DISPI_INDEX_BANK, BankNumber);
}