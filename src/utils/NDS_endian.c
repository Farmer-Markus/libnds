#include "NDS_endian.h"
#include <stdint.h>

/*
uint16_t NDS_ByteFlipU16(uint16_t val)
{
    uint8_t* valPtr = (uint8_t*)&val;
    val = valPtr[1] | (valPtr[0] << 8);

    return val;
}

uint32_t NDS_ByteFlipU32(uint32_t val)
{
    uint8_t* valPtr = (uint8_t*)&val;
    val = valPtr[3] | (valPtr[2] << 8) | (valPtr[1] << 16) | (valPtr[0] << 24); 

    return val;
}

uint64_t NDS_ByteFlipU64(uint64_t val)
{
    uint8_t* valPtr = (uint8_t*)&val;
    val = valPtr[7] | ((uint64_t)valPtr[6] << 8) | ((uint64_t)valPtr[5] << 16) | ((uint64_t)valPtr[4] << 24) | ((uint64_t)valPtr[3] << 32)
            | ((uint64_t)valPtr[2] << 40) | ((uint64_t)valPtr[1] << 48) | ((uint64_t)valPtr[0] << 56);


    return val;
}
*/
