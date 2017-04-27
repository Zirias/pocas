#include <windows.h>

#include "eventloop_internal.h"

    /*
SOEXPORT PlatformEvent *PlatformEvent_fromHandle(HANDLE handle, PlatformEventHandleAction action)
{
    PlatformEvent *self = malloc(sizeof(PlatformEvent));
    self->type = PET_Handle;
    self->peData.petHandle.handle = handle;
    self->peData.petHandle.action = action;
    if (action == PEHA_ReadOverlapped)
    {
        OverlappedReadData *ovrd = calloc(1, sizeof(OverlappedReadData));
        ovrd->pending = 0;
        ovrd->eof = 0;
        ovrd->over.hEvent = CreateEvent(0, FALSE, FALSE, 0);
        self->peData.petHandle.haData.ovRead = ovrd;
    }
    return self;
}

    */

SOEXPORT void PlatformEvent_destroy(PlatformEvent *self)
{
    /*
    if (self->type == PET_Handle)
    {
       if (self->peData.petHandle.action == PEHA_ReadOverlapped)
       {
           if (self->peData.petHandle.haData.ovRead->pending)
           {
               if (CancelIo(self->peData.petHandle.handle)
                       || GetLastError() != ERROR_NOT_FOUND)
               {
                   GetOverlappedResult(
                               self->peData.petHandle.handle,
                               &self->peData.petHandle.haData.ovRead->over,
                               0, TRUE);
               }
           }
           CloseHandle(self->peData.petHandle.haData.ovRead->over.hEvent);
           free(self->peData.petHandle.haData.ovRead);
       }
    }
    free(self);
    */
}
