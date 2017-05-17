#ifndef BQT_BACKEND_H
#define BQT_BACKEND_H

#include <pocas/core/eventloop.h>
#include <pocas/gui/private/backend.h>

#include <pocas/gui/backend/qt.h>

SOLOCAL_CDECL PC_EventProcessor Bqt_EventLoop_install();
SOLOCAL_CDECL void Bqt_Backend_init(void);
SOLOCAL_CDECL void Bqt_Control_setShown(void *control, int shown);
SOLOCAL_CDECL void Bqt_Control_setEnabled(void *control, int enabled);
SOLOCAL_CDECL void Bqt_Control_setContainer(void *control, void *container);
SOLOCAL_CDECL void Bqt_Control_setBounds(void *control, const PG_Bounds *b);
SOLOCAL_CDECL int Bqt_Window_create(PG_Window *w);
SOLOCAL_CDECL void Bqt_Window_close(PG_Window *w);
SOLOCAL_CDECL void Bqt_Window_destroy(PG_Window *w);
SOLOCAL_CDECL int Bqt_Label_create(PG_Label *l);
SOLOCAL_CDECL void Bqt_Label_setText(PG_Label *l, const char *text);
SOLOCAL_CDECL void Bqt_Label_destroy(PG_Label *l);
SOLOCAL_CDECL int Bqt_Button_create(PG_Button *b);
SOLOCAL_CDECL void Bqt_Button_setText(PG_Button *b, const char *text);
SOLOCAL_CDECL void Bqt_Button_destroy(PG_Button *b);

extern PG_Backend *PG_qtBackend;

#endif
