#ifndef NDS_rom_h
#define NDS_rom_h

#include <stdint.h>



#ifdef __cplusplus
extern "C"
{
#endif

// NDS Rom object
typedef struct NDS_Rom NDS_Rom;

#pragma pack(push, 1)
// NDS Rom header object
typedef struct NDS_RomHeader
{
    char gameTitle[12];
    char gameCode[4];
    char makerCode[2];
    uint8_t unitCode;
    uint8_t encryptionSeedSelect;
    uint8_t deviceCapacity;
    uint8_t reserved1[7];
    uint8_t reserved2;
    uint8_t ndsRegion;
    uint8_t romVersion;
    uint8_t autostart;
    uint32_t arm9RomOffset;
    uint32_t arm9EntryAddress;
    uint32_t arm9RamAddress;
    uint32_t arm9Size;
    uint32_t arm7RomOffset;
    uint32_t arm7EntryAddress;
    uint32_t arm7RamAddress;
    uint32_t arm7Size;
    uint32_t fntOffset;
    uint32_t fntSize;
    uint32_t fatOffset;
    uint32_t fatSize;
    uint32_t arm9FileOverlayOffset;
    uint32_t arm9FileOverlaySize;
    uint32_t arm7FileOverlayOffset;
    uint32_t arm7FileOverlaySize;
    uint32_t portSettingCommandsNormal;
    uint32_t portSettingCommandsKey1;
    uint32_t iconOffset;
    uint16_t secureAreaChecksum;
    uint16_t secureAreaDelay;
    uint32_t arm9AutoLoadListHookRamAddress;
    uint32_t arm7AutoLoadListHookRamAddress;
    uint64_t secureAreaDisable;
    uint32_t totalRomSize;
    uint32_t romHeaderSize;
    uint8_t unknown[4];
    uint8_t reserved3[8];
    uint16_t nandEndRomArea;
    uint16_t nandStartRwArea;
    uint8_t reserved4[24];
    uint8_t reserved5[16];
    uint8_t nintendoLogo[156];
    uint16_t nintendoLogoChecksum;
    uint16_t headerChecksum;
    uint32_t romDebugOffset;
    uint32_t debugSize;
    uint32_t debugRamAddress;
    uint8_t reserved6[4];
    uint8_t reserved7[144];
    uint8_t reserved8[3584];
} NDS_RomHeader;
#pragma pack(pop)

// Open NDS rom and return rom object
extern NDS_Rom* NDS_OpenRom(char* path);

// Close NDS rom from rom object
extern int NDS_CloseRom(NDS_Rom *rom);

// Get NDS rom header infos
extern const struct NDS_RomHeader* NDS_GetRomHeader(const NDS_Rom* rom);


#ifdef __cplusplus
}
#endif
#endif