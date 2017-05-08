#ifndef POCAS_GUI_MENU_H
#define POCAS_GUI_MENU_H

#include <pocas/gui/decl.h>

typedef struct Event Event;
typedef struct ListIterator ListIterator;
typedef struct Menu Menu;
typedef struct MenuItem MenuItem;
typedef struct Command Command;

DECLEXPORT Menu *Menu_create(void);
DECLEXPORT ListIterator *Menu_items(const Menu *self);
DECLEXPORT void Menu_addItem(Menu *self, MenuItem *item);
DECLEXPORT void Menu_removeItem(Menu *self, MenuItem *item);
DECLEXPORT void Menu_destroy(Menu *self);

DECLEXPORT MenuItem *MenuItem_create(const char *text);
DECLEXPORT const char *MenuItem_text(const MenuItem *self);
DECLEXPORT void MenuItem_setText(MenuItem *self, const char *text);
DECLEXPORT Command *MenuItem_command(const MenuItem *self);
DECLEXPORT void MenuItem_setCommand(MenuItem *self, Command *command);
DECLEXPORT void MenuItem_select(MenuItem *self);
DECLEXPORT Event *MenuItem_selectedEvent(const MenuItem *self);
DECLEXPORT Menu *MenuItem_subMenu(const MenuItem *self);
DECLEXPORT void MenuItem_setSubMenu(MenuItem *self, Menu *subMenu);
DECLEXPORT void MenuItem_destroy(MenuItem *self);

#endif
