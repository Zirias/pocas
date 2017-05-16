#ifndef POCAS_GUI_MENU_H
#define POCAS_GUI_MENU_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PC_Event);
C_CLASS_DECL(PC_ListIterator);

C_CLASS_DECL(PG_Command);

C_CLASS_DECL(PG_Menu);
C_CLASS_DECL(PG_MenuItem);

DECLEXPORT PG_Menu *PG_Menu_create(void);
DECLEXPORT PC_ListIterator *PG_Menu_items(const PG_Menu *self);
DECLEXPORT void PG_Menu_addItem(PG_Menu *self, PG_MenuItem *item);
DECLEXPORT void PG_Menu_removeItem(PG_Menu *self, PG_MenuItem *item);
DECLEXPORT void PG_Menu_destroy(PG_Menu *self);

DECLEXPORT PG_MenuItem *PG_MenuItem_create(const char *text);
DECLEXPORT const char *PG_MenuItem_text(const PG_MenuItem *self);
DECLEXPORT void PG_MenuItem_setText(PG_MenuItem *self, const char *text);
DECLEXPORT PG_Command *PG_MenuItem_command(const PG_MenuItem *self);
DECLEXPORT void PG_MenuItem_setCommand(PG_MenuItem *self, PG_Command *command);
DECLEXPORT void PG_MenuItem_select(PG_MenuItem *self);
DECLEXPORT PC_Event *PG_MenuItem_selectedEvent(const PG_MenuItem *self);
DECLEXPORT PG_Menu *PG_MenuItem_subMenu(const PG_MenuItem *self);
DECLEXPORT void PG_MenuItem_setSubMenu(PG_MenuItem *self, PG_Menu *subMenu);
DECLEXPORT void PG_MenuItem_destroy(PG_MenuItem *self);

#endif
