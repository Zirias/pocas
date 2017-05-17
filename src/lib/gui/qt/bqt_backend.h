#ifndef BQT_BACKEND_H
#define BQT_BACKEND_H

#include <pocas/core/eventloop.h>
#include <pocas/gui/private/backend.h>

#include <pocas/gui/backend/qt.h>

SOLOCAL_CDECL PC_EventProcessor Bqt_EventLoop_install();
SOLOCAL_CDECL void Bqt_Backend_init(void);
SOLOCAL_CDECL void Bqt_Control_setShown(void *control, int shown);
SOLOCAL_CDECL int Bqt_Window_create(PG_Window *w);
SOLOCAL_CDECL void Bqt_Window_close(PG_Window *w);

extern PG_Backend *PG_qtBackend;

#endif
