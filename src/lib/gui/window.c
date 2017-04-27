#include <stdlib.h>

#include <pocas/core/string.h>

#include "backend_internal.h"

#include <pocas/gui/window.h>

struct Window
{
    char *title;
    int width;
    int height;
    B_Window *b;
};

SOEXPORT Window *Window_create(const char *title, int width, int height)
{
    Window *self = malloc(sizeof(Window));
    self->title = String_copy(title);
    self->width = width;
    self->height = height;

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

SOEXPORT void Window_destroy(Window *self)
{
    if (!self) return;
    const Backend *b = Backend_current();
    if (b->Window_destroy) b->Window_destroy(self->b);
    free(self->title);
    free(self);
}
