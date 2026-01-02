#ifndef NDS_logging_h_
#define NDS_logging_h_

#include <stddef.h>
#ifdef NDS_DEBUG_BUILD
#define NDS_CHECK_PTR(ptr)                          \
    if(!ptr)                                        \
    {                                               \
        NDS_SetError("Given pointer is invalid!");  \
        return NULL;                                \
    }                               

#define NDS_CHECK_PTR_RINT(ptr)                     \
    if(!ptr)                                        \
    {                                               \
        NDS_SetError("Given pointer is invalid!");  \
        return -1;                                  \
    }
#else
#define NDS_CHECK_PTR(ptr)
#define NDS_CHECK_PTR_RINT(ptr)
#endif


typedef struct NDS_ErrorMessage
{
    size_t buff_size;
    size_t msg_len;
    char* message;

} NDS_ErrorMessage;

// %s, %i supported
int NDS_SetError(const char *format, ...);

// Set error message
//int NDS_SetError(const char *msg, ...);

// Get latest error message
const char* NDS_GetError(void);


#endif
