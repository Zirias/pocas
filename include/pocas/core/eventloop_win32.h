#ifndef EVENTLOOP_WIN32_H
#define EVENTLOOP_WIN32_H

#include <windows.h>

#include <pocas/core/eventloop.h>

C_CLASS_DECL(PC_Event);

DECLEXPORT PC_Event *PC_EventLoop_win32HndlEvent(void);
DECLEXPORT PC_Event *PC_EventLoop_win32MsgEvent(void);
DECLEXPORT void PC_EventLoop_setProcessMessages(int processMessages);

#endif // EVENTLOOP_WIN32_H
