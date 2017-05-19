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
SOLOCAL_CDECL void Bqt_Control_focus(void *control);
SOLOCAL_CDECL int Bqt_Window_create(PG_Window *w);
SOLOCAL_CDECL void Bqt_Window_setMenu(PG_Window *w, PG_Menu *menu);
SOLOCAL_CDECL void Bqt_Window_close(PG_Window *w);
SOLOCAL_CDECL void Bqt_Window_showModal(PG_Window *w);
SOLOCAL_CDECL void Bqt_Window_hideModal(PG_Window *w);
SOLOCAL_CDECL void Bqt_Window_destroy(PG_Window *w);
SOLOCAL_CDECL int Bqt_Label_create(PG_Label *l);
SOLOCAL_CDECL void Bqt_Label_setText(PG_Label *l, const char *text);
SOLOCAL_CDECL void Bqt_Label_destroy(PG_Label *l);
SOLOCAL_CDECL int Bqt_Button_create(PG_Button *b);
SOLOCAL_CDECL void Bqt_Button_setText(PG_Button *b, const char *text);
SOLOCAL_CDECL void Bqt_Button_destroy(PG_Button *b);
SOLOCAL_CDECL int Bqt_TextBox_create(PG_TextBox *tb, PG_TextBox_textChanged changed);
SOLOCAL_CDECL void Bqt_TextBox_setText(PG_TextBox *tb, const char *text);
SOLOCAL_CDECL void Bqt_TextBox_destroy(PG_TextBox *tb);
SOLOCAL_CDECL PG_MessageBoxButton Bqt_MessageBox_show(const PG_Window *w, const char *title,
        const char *text, PG_MessageBoxButton buttons, PG_MessageBoxStyle style);
SOLOCAL_CDECL int Bqt_MenuItem_create(PG_MenuItem *mi);
SOLOCAL_CDECL void Bqt_MenuItem_destroy(PG_MenuItem *mi);
SOLOCAL_CDECL int Bqt_Menu_create(PG_Menu *m);
SOLOCAL_CDECL void Bqt_Menu_addItem(PG_Menu *m, PG_MenuItem *item);
SOLOCAL_CDECL void Bqt_Menu_removeItem(PG_Menu *m, PG_MenuItem *item);
SOLOCAL_CDECL void Bqt_Menu_destroy(PG_Menu *m);

extern PG_Backend *PG_qtBackend;

#endif
