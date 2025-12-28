#ifndef NDS_logging_h_
#define NDS_logging_h_

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


typedef struct NDS_ErrorMessage
{
    char* message;

} NDS_ErrorMessage;

// Set error message
int NDS_SetError(const char* msg);

// Get latest error message
const char* NDS_GetError(void);


#endif
