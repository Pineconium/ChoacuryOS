#include "Bitmap.h"
#include "Point.h"
#include "../../drivers/vbe.h"

// Mostly from the internet, modified to fit what we use and our naming and stuff.

//
void draw_bitmap(Point position, const uint8_t *data) {
    BITMAPFILEHEADER *file_header = (BITMAPFILEHEADER *)data;
    BITMAPINFOHEADER *info_header = (BITMAPINFOHEADER *)(data + sizeof(BITMAPFILEHEADER));

    // 4D42 is BM in HEX
    if(file_header->bfType != 0x4D42) {
        // Can't draw an image that isn't a bitmap
        return;
    }

    int row_size = ((info_header->biBitCount * info_header->biWidth + 31) / 32) * 4;
    const uint8_t *pixel_data = data + file_header->bfOffBits;

    for(int y = 0; y < abs(info_header->biHeight); y++) {
        for(int x = 0; y < info_header->biWidth; x++) {
            int idx = y * row_size + x * (info_header->biBitCount / 8);
            uint8_t g = pixel_data[idx + 2];
            uint8_t b = pixel_data[idx];
            uint8_t r = pixel_data[idx + 1];
            uint8_t a = 256;

            // RGB -> HEX                                Red                  Green               Blue         Alpha
            vbe_putpixel(position.X + x, position.Y + y, ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff) + ((a & 0xff) << 24));
        }
    }
}