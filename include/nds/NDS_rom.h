#ifndef NDS_rom_h
#define NDS_rom_h

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

// NDS Rom object
typedef struct NDS_Rom NDS_Rom;

// NDS Rom header object
#pragma pack(push, 1)
typedef struct NDS_RomHeader
{
    char game_title[0x0C];
    char game_code[0x04];
    char maker_code[0x02];

    uint8_t unit_code;
    uint8_t encryption_seed_select;
    uint8_t device_capacity;

    uint8_t reserved1[0x07];
    uint8_t reserved2;

    uint8_t nds_region;
    uint8_t rom_version;
    uint8_t autostart;

    uint32_t arm9_rom_offset;
    uint32_t arm9_entry_address;
    uint32_t arm9_ram_address;
    uint32_t arm9_size;
    uint32_t arm7_rom_offset;
    uint32_t arm7_entry_address;
    uint32_t arm7_ram_address;
    uint32_t arm7_size;

    uint32_t fnt_offset;
    uint32_t fnt_size;
    uint32_t fat_offset;
    uint32_t fat_size;

    uint32_t arm9_file_overlay_offset;
    uint32_t arm9_file_overlay_size;
    uint32_t arm7_file_overlay_offset;
    uint32_t arm7_file_overlay_size;

    uint32_t port_setting_commands_normal;
    uint32_t port_setting_commands_key1;

    uint32_t icon_offset;

    uint16_t secure_area_checksum;
    uint16_t secure_area_delay;

    uint32_t arm9_auto_load_list_hook_ram_address;
    uint32_t arm7_auto_load_list_hook_ram_address;

    uint32_t secure_area_disable[2];

    uint32_t total_rom_size;
    uint32_t rom_header_size;

    uint8_t unknown[0x4];
    uint8_t reserved3[0x8];

    uint16_t nand_end_rom_area;
    uint16_t nand_start_rw_area;

    uint8_t reserved4[0x18];
    uint8_t reserved5[0x10];

    uint8_t nintendo_logo[0x9C];
    uint16_t nintendo_logo_checksum;
    uint16_t header_checksum;

    uint32_t rom_debug_offset;
    uint32_t debug_size;
    uint32_t debug_ram_address;

    uint8_t reserved6[0x04];
    uint8_t reserved7[0x90];
    uint8_t reserved8[0xE00];
} NDS_RomHeader;
#pragma pack(pop)

// Open NDS rom
// object pointer on success, NULL on failure
extern NDS_Rom* NDS_RomOpen(const char *path);

// Close NDS rom object
// 0 on success, -1 on failure
extern int NDS_RomClose(NDS_Rom *romptr);

// Get NDS rom header info object
// object pointer on success, NULL on failure
extern const struct NDS_RomHeader* NDS_RomGetHeader(const NDS_Rom *rom);

#ifdef __cplusplus
}
#endif
#endif
