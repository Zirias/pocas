#ifndef POCAS_GUI_WINDOW_H
#define POCAS_GUI_WINDOW_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PC_Event);

C_CLASS_DECL(PG_Menu);

C_CLASS_DECL(PG_Window);

DECLEXPORT PG_Window *PG_Window_create(PG_Window *parent, const char *title, int width, int height);
DECLEXPORT PG_Window *PG_Window_parent(const PG_Window *self);
DECLEXPORT const char *PG_Window_title(const PG_Window *self);
DECLEXPORT int PG_Window_width(const PG_Window *self);
DECLEXPORT int PG_Window_height(const PG_Window *self);
DECLEXPORT PG_Menu *PG_Window_menu(const PG_Window *self);
DECLEXPORT void PG_Window_setMenu(PG_Window *self, PG_Menu *menu);
DECLEXPORT void PG_Window_close(PG_Window *self);
DECLEXPORT void PG_Window_showModal(PG_Window *self);
DECLEXPORT void PG_Window_hideModal(PG_Window *self);
DECLEXPORT void *PG_Window_executeDialog(PG_Window *self);
DECLEXPORT void PG_Window_closeDialog(PG_Window *self, void *result);
DECLEXPORT PC_Event *PG_Window_dialogShownEvent(const PG_Window *self);
DECLEXPORT PC_Event *PG_Window_closingEvent(const PG_Window *self);
DECLEXPORT void PG_Window_destroy(PG_Window *self);

DECLEXPORT PC_Event *PG_Window_lastWindowClosedEvent(void);

#endif
