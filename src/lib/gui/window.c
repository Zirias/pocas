#include <stdlib.h>

#include <pocas/core/string.h>

#include "backend_internal.h"

#include <pocas/gui/menu.h>
#include <pocas/gui/window.h>

struct Window
{
    B_Window *b;
    char *title;
    int width;
    int height;
    Menu *menu;
};

SOEXPORT Window *Window_create(const char *title, int width, int height)
{
    Window *self = malloc(sizeof(Window));
    self->title = String_copy(title);
    self->width = width;
    self->height = height;
    self->menu = 0;

    const Backend *b = Backend_current();
    self->b = b->Window_create ? b->Window_create(self) : 0;
    return self;
}

SOEXPORT const char *Window_title(const Window *self)
{
    return self->title;
}

SOEXPORT int Window_width(const Window *self)
{
    return self->width;
}

SOEXPORT int Window_height(const Window *self)
{
    return self->height;
}

SOEXPORT void Window_show(Window *self)
{
    const Backend *b = Backend_current();
    if (b->Window_show) b->Window_show(self->b);
}

SOEXPORT void Window_hide(Window *self)
{
    const Backend *b = Backend_current();
    if (b->Window_hide) b->Window_hide(self->b);
}

SOEXPORT Menu *Window_menu(const Window *self)
{
    return self->menu;
}

SOEXPORT void Window_setMenu(Window *self, Menu *menu)
{
    Menu_destroy(self->menu);
    self->menu = menu;

    const Backend *b = Backend_current();
    if (b->Window_setMenu) b->Window_setMenu(self->b, ((Frontend *)menu)->b);
}

SOEXPORT void Window_destroy(Window *self)
{
    if (!self) return;
    const Backend *b = Backend_current();
    if (b->Window_destroy) b->Window_destroy(self->b);
    free(self->title);
    free(self);
}