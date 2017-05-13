#include "c11threads.h"

#include <stdlib.h>

#include <pocas/core/event.h>
#include <pocas/core/eventloop.h>
#include <pocas/core/string.h>

#include "internal.h"

#include <pocas/gui/control.h>
#include <pocas/gui/menu.h>
#include <pocas/gui/window.h>

struct Window
{
    GuiClass gc;
    Window *parent;
    Event *closing;
    Event *dialogShown;
    int closed;
    int dialog;
    void *dialogResult;
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
    self->dialogShown = Event_create("dialogShown");
    self->title = String_copy(title);
    self->width = width;
    self->height = height;
    self->menu = 0;
    self->dialog = 0;
    self->dialogResult = 0;

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
    EventArgs args = EventArgs_init(self->closing, self, 0);
    Event_raise(self->closing, &args);
    if (!args.handled)
    {
        const Backend *b = Backend_current();
        if (b->backendApi.window.close) b->backendApi.window.close(self);
        self->closed = 1;
    }
}

SOEXPORT void *Window_showDialog(Window *self)
{
    if (!self->parent) return 0;
    self->dialog = 1;
    Control_show(self);
    Control_disable(self->parent);
    EventArgs args = EventArgs_init(self->dialogShown, self, 0);
    Event_raise(self->dialogShown, &args);
    while (self->dialog) EventLoop_processEvents(-1);
    Control_enable(self->parent);
    Control_hide(self);
    return self->dialogResult;
}

SOEXPORT void Window_closeDialog(Window *self, void *result)
{
    self->dialogResult = result;
    self->dialog = 0;
}

SOEXPORT Event *Window_closingEvent(const Window *self)
{
    return self->closing;
}

SOEXPORT Event *Window_dialogShownEvent(const Window *self)
{
    return self->dialogShown;
}

SOEXPORT void Window_destroy(Window *self)
{
    if (!self) return;
    const Backend *b = Backend_current();
    if (b->backendApi.window.destroy) b->backendApi.window.destroy(self);
    Event_destroy(self->dialogShown);
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
