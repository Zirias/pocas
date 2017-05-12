#ifndef EVENTLOOP_WIN32_H
#define EVENTLOOP_WIN32_H

#include <windows.h>

#include <pocas/core/eventloop.h>

typedef struct Event Event;

DECLEXPORT Event *EventLoop_win32HndlEvent(void);
DECLEXPORT Event *EventLoop_win32MsgEvent(void);
DECLEXPORT Event *EventLoop_win32RawMsgEvent(void);
DECLEXPORT LRESULT CALLBACK EventLoop_win32WndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp);
DECLEXPORT void EventLoop_setProcessMessages(int processMessages);

#endif // EVENTLOOP_WIN32_H
