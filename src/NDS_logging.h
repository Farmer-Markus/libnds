#ifndef NDS_LOGGING_H_INTERNAL
#define NDS_LOGGING_H_INTERNAL


typedef struct NDS_ErrorMessage
{
    char* message;

} NDS_ErrorMessage;

// Set error message
int NDS_SetError(const char* msg);

// Get latest error message
const char* NDS_GetError(void);


#endif