#include "pl.h"

#include "../../terminal.h"
#include "../../../drivers/vga.h"
#include "../../../drivers/vbe.h"
#include "../../../drivers/storage/device.h"

int shell_pl_command(int argc, const char** argv) {
    term_write("Detected Devices: ", TC_WHITE);
    term_write_u32(g_storage_device_count, 10, TC_WHITE);
    term_write("\n", TC_WHITE);
    for (int i = 0; i < g_storage_device_count; i++) {
        storage_device_t* device = g_storage_devices[i];
        u64 device_size = device->sector_count * device->sector_size;

        term_write("  ", TC_WHITE);
        term_write(device->model, TC_WHITE);
        term_write(" (", TC_WHITE);
        term_write_u32(device_size / 1024 / 1024, 10, TC_WHITE);
        term_write(" MiB)\n", TC_WHITE);

        for (u32 j = 0; j < device->partition_count; j++) {
            storage_device_t* partition = device->partitions[j];
            u64 partition_size = partition->sector_count * partition->sector_size;

            term_write("  \xC0 ", TC_WHITE);        // <- Stylistic choice.
                                                        // TOFIX: Use Hex C4 (├) for any other partition
                                                        // and save Hex C0 (└) for that last partition in each list
            term_write(partition->model, TC_WHITE);
            term_write(" (", TC_WHITE);
            term_write_u32(partition_size / 1024 / 1024, 10, TC_WHITE);
            term_write(" MiB)\n", TC_WHITE);
        }
    }

    return 0;
}