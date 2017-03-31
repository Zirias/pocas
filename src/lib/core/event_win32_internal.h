#ifndef EVENT_WIN32_INTERNAL_H
#define EVENT_WIN32_INTERNAL_H

#include <windows.h>

#include <pocas/core/event_win32.h>

#define ORD_BUFSIZE 4096
typedef struct OverlappedReadData
{
    OVERLAPPED over;
    int pending;
    int eof;
    char buffer[ORD_BUFSIZE];
} OverlappedReadData;

struct PlatformEvent
{
    PlatformEventType type;
    union
    {
        struct
        {
            HANDLE handle;
            PlatformEventHandleAction action;
            union
            {
                OverlappedReadData *ovRead;
            } haData;
        } petHandle;
    } peData;
};

#endif
