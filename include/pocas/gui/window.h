#ifndef POCAS_GUI_WINDOW_H
#define POCAS_GUI_WINDOW_H

#include <pocas/gui/decl.h>

typedef struct Window Window;

DECLEXPORT Window *Window_create(const char *title, int width, int height);
DECLEXPORT const char *Window_title(const Window *self);
DECLEXPORT int Window_width(const Window *self);
DECLEXPORT int Window_height(const Window *self);
DECLEXPORT void Window_show(Window *self);
DECLEXPORT void Window_hide(Window *self);
DECLEXPORT void Window_destroy(Window *self);

#endif
