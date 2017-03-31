#ifndef POCAS_CORE_EVENT_WIN32_H
#define POCAS_CORE_EVENT_WIN32_H

#include <windows.h>

#include <pocas/core/event.h>

typedef enum PlatformEventType
{
    PET_Handle
} PlatformEventType;

typedef enum PlatformEventHandleAction
{
    PEHA_ReadOverlapped
} PlatformEventHandleAction;

DECLEXPORT PlatformEvent *PlatformEvent_fromHandle(HANDLE handle, PlatformEventHandleAction action);

#endif
