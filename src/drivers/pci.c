#include "pci.h"
#include "types.h"
#include "ports.h"
#include "debug.h"

u32 read_pci(u16 bus, u16 device, u16 function, u32 regoffset) {
    //create ID to identify an exact function and register offset we want to start reading from (the PCI has 8 buses, 32 devices each, where each device has up to 8 functions)
    //creating it is easy, you pretty much put the plain values there in order, so they're all shifted and ORed with each other
    u32 id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (regoffset & 0xFC);
    port_dword_out(0xCF8, id); //give ID to PCI's command port
    u32 result = port_dword_in(0xCFC); //read the result from PCI's data port
    return result >> ((regoffset % 4) << 3); //the real result beginning from a register offset has to be shifted
}

void write_pci(u16 bus, u16 device, u16 function, u32 regoffset, u32 data) {
    u32 id = 0x1 << 31 | ((bus & 0xFF) << 16) | ((device & 0x1F) << 11) | ((function & 0x07) << 8) | (regoffset & 0xFC); //we construct an ID like in the read function
    port_dword_out(0xCF8, id); //we give the ID to PCI's command port
    port_dword_out(0xCFC, data); //we give the data we want to send to PCI's data port
}

device_descriptor get_device_descriptor(u16 bus, u16 device, u16 function) {
    device_descriptor result;

    result.bus = bus;
    result.device = device;
    result.function = function;

    result.vendor_id = read_pci(bus, device, function, 0x00);
    result.device_id = read_pci(bus, device, function, 0x02);

    result.class_id = read_pci(bus, device, function, 0x0b);
    result.subclass_id = read_pci(bus, device, function, 0x0a);
    result.interface_id = read_pci(bus, device, function, 0x09);

    result.revision = read_pci(bus, device, function, 0x08);
    result.interrupt = read_pci(bus, device, function, 0x3c);

    return result;
}

void debug_print_pci() {
    for(int bus = 0; bus < 8; bus++) //8 buses
    {
        for(int device = 0; device < 32; device++) //32 possible devices
        {
            for(int function = 0; function < 8; function++) //8 possible functionss
            {
                device_descriptor desc = get_device_descriptor(bus, device, function);

                if(desc.vendor_id == 0x0000 || desc.vendor_id == 0xFFFF) //if the vendor ID is 0 or MAX then the function doesn't exist
                    continue;

                dprint("PCI bus: ");
                dprintbyte((u8)(bus & 0xFF));

                dprint(", Device: ");
                dprintbyte((u8)(device & 0xFF));

                dprint(", Function: ");
                dprintbyte(((u8)((function & 0xFF))));

                dprint(", Vendor ID: ");
                dprintbyte((u8)((desc.vendor_id & 0xFF00) >> 8));
                dprintbyte((u8)(desc.vendor_id & 0xFF));

                dprint(", Device ID: ");
                dprintbyte((u8)((desc.device_id & 0xFF00) >> 8));
                dprintbyte((u8)(desc.device_id & 0xFF));

                dprint("\r\n");
            }
        }
    }
}