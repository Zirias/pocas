#ifndef EVENTLOOP_WIN32_H
#define EVENTLOOP_WIN32_H

#include <windows.h>

#include <pocas/core/eventloop.h>

typedef struct Event Event;

DECLEXPORT Event *Eventloop_win32HndlEvent(void);
DECLEXPORT Event *Eventloop_win32MsgEvent(void);
DECLEXPORT LRESULT CALLBACK Eventloop_win32WndProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp);

#endif // EVENTLOOP_WIN32_H
