#include <NDS_memory.h>
#include <NDS_logging.h>
#include <stddef.h>
#include <stdlib.h>

void* NDS_Malloc(size_t size)
{
    void* ptr = malloc(size);
    if(ptr)
        return ptr;

    NDS_SetError("Failed to malloc! Is your memory full?");
    return NULL;
}