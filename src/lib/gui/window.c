#include "c11threads.h"

#include <stdlib.h>

#include <pocas/core/event.h>
#include <pocas/core/string.h>

#include "internal.h"

#include <pocas/gui/menu.h>
#include <pocas/gui/window.h>

struct Window
{
    GuiClass gc;
    Window *parent;
    Event *closing;
    int closed;
    char *title;
    int width;
    int height;
    Menu *menu;
};

SOEXPORT Window *Window_create(Window *parent, const char *title, int width, int height)
{
    Window *self = malloc(sizeof(Window));
    GCINIT(self);
    privateApi.container.create(self);
    privateApi.control.create(self);
    self->parent = parent;
    self->closed = 0;
    self->closing = Event_create("closing");
    self->title = String_copy(title);
    self->width = width;
    self->height = height;
    self->menu = 0;

    const Backend *b = Backend_current();
    if (b->backendApi.window.create) b->backendApi.window.create(self);
    return self;
}

SOEXPORT Window *Window_parent(const Window *self)
{
    return self->parent;
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

SOEXPORT Menu *Window_menu(const Window *self)
{
    return self->menu;
}

SOEXPORT void Window_setMenu(Window *self, Menu *menu)
{
    Menu_destroy(self->menu);
    self->menu = menu;

    const Backend *b = Backend_current();
    if (b->backendApi.window.setMenu) b->backendApi.window.setMenu(self, menu);
}

SOEXPORT void Window_close(Window *self)
{
    EventArgs *args = EventArgs_create(self->closing, self, 0);
    Event_raise(self->closing, args);
    if (!EventArgs_handled(args))
    {
        const Backend *b = Backend_current();
        if (b->backendApi.window.close) b->backendApi.window.close(self);
        self->closed = 1;
    }
    EventArgs_destroy(args);
}

SOEXPORT Event *Window_closingEvent(const Window *self)
{
    return self->closing;
}

SOEXPORT void Window_destroy(Window *self)
{
    if (!self) return;
    const Backend *b = Backend_current();
    if (b->backendApi.window.destroy) b->backendApi.window.destroy(self);
    Event_destroy(self->closing);
    free(self->title);
    privateApi.control.destroy(self);
    privateApi.container.destroy(self);
    free(self);
}

static thread_local Event *lastWindowClosed = 0;

SOEXPORT Event *Window_lastWindowClosedEvent(void)
{
    if (!lastWindowClosed)
    {
        lastWindowClosed = Event_create("lastWindowClosed");
    }
    return lastWindowClosed;
}
