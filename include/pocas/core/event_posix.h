#ifndef POCAS_CORE_EVENT_POSIX_H
#define POCAS_CORE_EVENT_POSIX_H

#include <pocas/core/event.h>

typedef enum PlatformEventType
{
    PET_Fd
} PlatformEventType;

typedef enum PlatformEventHandleAction
{
    PEFD_SelectRead
} PlatformEventHandleAction;

DECLEXPORT PlatformEvent *PlatformEvent_fromFd(int fd, PlatformEventHandleAction action);

#endif
