#ifndef POCAS_GUI_WINDOW_H
#define POCAS_GUI_WINDOW_H

#include <pocas/gui/decl.h>

typedef struct Event Event;
typedef struct Menu Menu;

typedef struct Window Window;

DECLEXPORT Window *Window_create(Window *parent, const char *title, int width, int height);
DECLEXPORT Window *Window_parent(const Window *self);
DECLEXPORT const char *Window_title(const Window *self);
DECLEXPORT int Window_width(const Window *self);
DECLEXPORT int Window_height(const Window *self);
DECLEXPORT Menu *Window_menu(const Window *self);
DECLEXPORT void Window_setMenu(Window *self, Menu *menu);
DECLEXPORT void Window_close(Window *self);
DECLEXPORT Event *Window_closingEvent(const Window *self);
DECLEXPORT void Window_destroy(Window *self);

DECLEXPORT Event *Window_lastWindowClosedEvent(void);

#endif
