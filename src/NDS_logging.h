#ifndef NDS_logging_h_
#define NDS_logging_h_


typedef struct NDS_ErrorMessage
{
    char* message;

} NDS_ErrorMessage;

// Set error message
int NDS_SetError(const char* msg);

// Get latest error message
const char* NDS_GetError(void);


#endif