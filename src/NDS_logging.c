// SPDX-License-Identifier: GPL-3.0-only
#include "NDS_logging.h"
#include "NDS_memory.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


/*
  This all needs to be rewritten thread save & cleaned up & just made better

*/


// Get global error message struct
static NDS_ErrorMessage* NDS_GetGlobalErrorMessage(void);
// Reallocate memory and set error message
static int NDS_ReallocErrorMsg(NDS_ErrorMessage* ndsMsg, const char* msg);


NDS_ErrorMessage* NDS_GetGlobalErrorMessage(void)
{
    static NDS_ErrorMessage errMsg = {.buff_size = 0, .msg_len = 0};
    return &errMsg;
}

int NDS_SetError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    NDS_ErrorMessage *errMsg = NDS_GetGlobalErrorMessage();

    bool found = false;
    for(char c = 0x1; c != 0x0; format++)
    {
        c = *format;
        if(found)
        {
            switch(c)
            {
                case 's': // cstring
                    if(NDS_ReallocErrorMsg(errMsg, va_arg(args, const char*)) != 0)
                        return -1;
                    break;

                case 'i': // integer
                {
                    // Convert to cstr, store in buffer and write to logmsg
                    char imsg_buff[20];
                    sprintf(imsg_buff, "%d", va_arg(args, const int));
                    if(NDS_ReallocErrorMsg(errMsg, imsg_buff) != 0)
                        return -1;
                    break;
                }

                default:
                    break;
            }
            found = false;
            continue;
        }

        if(c == '%')
        {
            found = true;
            continue;
        }
        
        char buff[2] = {0};
        buff[0] = c;
        NDS_ReallocErrorMsg(errMsg, buff);
    }

    return 0;
}



/*int NDS_SetError(const char *msg, ...)
{
    va_list list;
    
    for(va_start(list, msg); *msg != 0x0; msg++)
    {
        va_arg(3);
    }

    // Make thread save!!!!
    NDS_ErrorMessage* errMsg = NDS_GetGlobalErrorMessage();
    NDS_ReallocErrorMsg(errMsg, msg);

    return 0;
}*/

const char* NDS_GetError(void)
{
    NDS_ErrorMessage* errMsg = NDS_GetGlobalErrorMessage();
    if(errMsg->message)
        return errMsg->message;
    return "";
}


static int NDS_ReallocErrorMsg(NDS_ErrorMessage* nds_msg, const char *msg)
{
    size_t msg_len = strlen(msg);

    // Reallocate
    size_t new_len = nds_msg->msg_len + msg_len;
    if(new_len >= nds_msg->buff_size)
    {
        size_t new_buff_size = new_len * 2;
        // Rather to much than constant reallocating
        char *new = NDS_Malloc(new_buff_size);
        if(!new) return -1;
        // Copy old msg over to new memory
        if(nds_msg->msg_len > 0)
            strcpy(new, nds_msg->message);

        // Free old ptr and replace with new
        free(nds_msg->message);
        nds_msg->message = new;
        nds_msg->buff_size = new_buff_size;
    }

    // And now append new msg
    strcpy(&nds_msg->message[nds_msg->msg_len], msg);
    nds_msg->msg_len = new_len;
    return 0;
}

/*
static int NDS_ReallocErrorMsg(NDS_ErrorMessage* ndsMsg, const char* msg)
{
    size_t msgLen = strlen(msg);
    ndsMsg->message = (char*)realloc(ndsMsg->message, msgLen + 1);
    strcpy(ndsMsg->message, msg);
    ndsMsg->message[msgLen] = 0; // 0 terminator at end

    return 0;
}
*/