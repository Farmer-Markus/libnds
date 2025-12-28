#include "NDS_logging.h"
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

// Get global error message struct
static NDS_ErrorMessage* NDS_GetGlobalErrorMessage(void);
// Reallocate memory and set error message
static int NDS_ReallocErrorMsg(NDS_ErrorMessage* ndsMsg, const char* msg);



NDS_ErrorMessage* NDS_GetGlobalErrorMessage(void)
{
    static NDS_ErrorMessage errMsg;
    return &errMsg;
}

int NDS_SetError(const char* msg)
{
    NDS_ErrorMessage* errMsg = NDS_GetGlobalErrorMessage();
    NDS_ReallocErrorMsg(errMsg, msg);

    return 0;
}

const char* NDS_GetError(void)
{
    NDS_ErrorMessage* errMsg = NDS_GetGlobalErrorMessage();
    if(errMsg->message)
        return errMsg->message;
    return "";
}

static int NDS_ReallocErrorMsg(NDS_ErrorMessage* ndsMsg, const char* msg)
{
    size_t msgLen = strlen(msg);
    ndsMsg->message = (char*)realloc(ndsMsg->message, msgLen + 1);
    strcpy(ndsMsg->message, msg);
    ndsMsg->message[msgLen] = 0; // 0 terminator at end

    return 0;
}