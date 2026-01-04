// SPDX-License-Identifier: GPL-3.0-only
#ifndef NDS_rom_info_h_
#define NDS_rom_info_h_

#include "NDS_rom.h"
#include <stdint.h>


typedef struct NDS_RomInfo NDS_RomInfo;

#pragma pack(push, 1)
struct NDS_RomInfo
{
    uint16_t version;
    uint16_t crc16_all;
    uint16_t crc16_2_up;
    uint16_t crc16_3_up;
    uint16_t crc16_259_up;

    uint8_t reserved[0x16];

    uint8_t icon_data[0x200]; // 32*32 px, 4*4 tiles, 8*8 px/tile
    uint16_t palette_data[0x10]; // 16 colors, 0000-7FFF, 0 = transparent

    uint16_t uni_title_jp[0x80]; // 16 bit unicode
    uint16_t uni_title_en[0x80];
    uint16_t uni_title_fr[0x80];
    uint16_t uni_title_de[0x80];
    uint16_t uni_title_it[0x80];
    uint16_t uni_title_es[0x80];
    uint16_t uni_title_cn[0x80]; // ver 0002 & up
    uint16_t uni_title_kr[0x80]; // ver 0003 & up

    uint8_t unknown1[0x800];
};
#pragma pack(pop)

// Get NDS rom info object
// 0 on success, -1 on failure
int NDS_RomInfoGet(NDS_RomInfo *dest, const NDS_Rom *rom);


#endif