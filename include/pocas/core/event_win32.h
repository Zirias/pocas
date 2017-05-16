#ifndef POCAS_CORE_EVENT_WIN32_H
#define POCAS_CORE_EVENT_WIN32_H

#include <windows.h>

#include <pocas/core/event.h>

C_CLASS_DECL(PC_Win32HndlEvInfo);
struct PC_Win32HndlEvInfo
{
    HANDLE hndl;
    void *owner;
};

C_CLASS_DECL(PC_Win32MsgEvInfo);
struct PC_Win32MsgEvInfo
{
    HWND wnd;
    UINT msg;
    WPARAM wp;
    LPARAM lp;
    LRESULT result;
};

#endif
