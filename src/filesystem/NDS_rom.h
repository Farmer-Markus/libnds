#ifndef NDS_rom_h_
#define NDS_rom_h_

#include <stdint.h>
#include <stdio.h>

#define NDS_RH_GAMETITLE_SIZE 12
#define NDS_RH_GAMECODE_SIZE 4
#define NDS_RH_MAKERCODE_SIZE 2


#pragma pack(push, 1)
typedef struct NDS_RomHeader
{   // https://problemkaputt.de/gbatek.htm#dscartridgeheader
    char gameTitle[NDS_RH_GAMETITLE_SIZE];
    char gameCode[NDS_RH_GAMECODE_SIZE];
    char makerCode[NDS_RH_MAKERCODE_SIZE];
    uint8_t unitCode;
    uint8_t encryptionSeedSelect;
    uint8_t deviceCapacity;
    uint8_t reserved1[7];       // zero filled
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
    uint8_t reserved6[4];       // zero filled
    uint8_t reserved7[144];     // zero filled
    uint8_t reserved8[3584];    // zero filled
} NDS_RomHeader;
#pragma pack(pop)


typedef struct NDS_Rom
{
    NDS_RomHeader r_header;
    FILE* r_fstream;

} NDS_Rom;


// Open rom object from path
// return NULL on fail
NDS_Rom* NDS_OpenRom(char *path);

// Close/Destroy rom object
// 0 on Success, -1 Error
int NDS_CloseRom(NDS_Rom *romptr);

const NDS_RomHeader* NDS_GetRomHeader(const NDS_Rom *romptr);


#endif
