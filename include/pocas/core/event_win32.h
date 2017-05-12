#ifndef POCAS_CORE_EVENT_WIN32_H
#define POCAS_CORE_EVENT_WIN32_H

#include <windows.h>

#include <pocas/core/event.h>

typedef struct Win32HndlEvInfo
{
    HANDLE hndl;
    void *owner;
} Win32HndlEvInfo;

typedef struct Win32MsgEvInfo
{
    HWND wnd;
    UINT msg;
    WPARAM wp;
    LPARAM lp;
    LRESULT result;
} Win32MsgEvInfo;

#endif
