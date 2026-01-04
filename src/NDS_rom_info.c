#include "NDS_rom_info.h"
#include "NDS_logging.h"
#include "NDS_rom.h"

#define ICON_PX_W 0x20 // 32px wide
#define ICON_TILE_PX_W 0x8 // 8px wide
#define ICON_TOTAL_PX 0x400


static uint32_t NDS_Bgr16ToRgba32(uint16_t col);


int NDS_RomInfoGet(NDS_RomInfo *dest, const NDS_Rom *rom)
{
    NDS_CHECK_PTR_RINT(rom);
    if(NDS_RomRead(dest, rom->r_header.icon_offset, sizeof(NDS_RomInfo), &rom->acc) != 0)
        return -1;

    return 0;
}


// Where to implement this?? In rominfo or another file?
// Size will be (icon_size * 2 * 4) = 0x1000 bytes
int NDS_CIconToRgba32(void *dest, const NDS_RomInfo *info)
{
    NDS_CHECK_PTR_RINT(icon);

    uint32_t *final_buff = (uint32_t*)dest;
    uint16_t src_off = 0;
    for(uint16_t tile_y = 0; tile_y < ICON_PX_W; tile_y += ICON_TILE_PX_W)
    {
        for(uint16_t tile_x = 0; tile_x < ICON_PX_W; tile_x += ICON_TILE_PX_W)
        {
            for(uint8_t px_y = 0; px_y < ICON_TILE_PX_W; px_y++)
            {
                for(uint8_t px_x = 0; px_x < ICON_TILE_PX_W; px_x += 2)
                {
                    // Each byte contains 2 pixels beginning on the right bit
                    uint8_t byte = info->icon_data[src_off++];
                    uint8_t pix1 = byte & 0x0F;
                    uint8_t pix2 = (byte >> 4) & 0x0F;
                    
                    uint16_t dest_off = (tile_y + px_y) * ICON_PX_W + (tile_x + px_x);
                    final_buff[dest_off] = NDS_Bgr16ToRgba32(info->palette_data[pix1]);
                    final_buff[dest_off + 1] = NDS_Bgr16ToRgba32(info->palette_data[pix2]);
                }
            }
        }
    }

    return 0;
}

// bgr 16 bit -> rgba 32 bit
static uint32_t NDS_Bgr16ToRgba32(uint16_t col)
{
    // And scale color up to fit with 8 bits
    uint8_t r = (col & 0x1F) << 3;
    uint8_t g = ((col >> 5) & 0x1F) << 3;
    uint8_t b = ((col >> 10) & 0x1F) << 3;
    uint8_t a = col == 0 ? 0 : 0xFF;

    // Little endian highest byte is on the right so we just make it the biggest byte
    return (a << 24) | (b << 16) | (g << 8) | r; // rgba
    //return (a << 24) | (r << 16) | (g << 8) | b; // bgra
}