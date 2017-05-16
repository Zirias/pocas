#include "c11threads.h"

#include <stdlib.h>

#include <pocas/core/event.h>
#include <pocas/core/eventloop.h>
#include <pocas/core/string.h>

#include "internal.h"

#include <pocas/gui/control.h>
#include <pocas/gui/menu.h>
#include <pocas/gui/window.h>

struct PG_Window
{
    GuiClass gc;
    PG_Window *parent;
    PC_Event *closing;
    PC_Event *dialogShown;
    int closed;
    int dialog;
    void *dialogResult;
    char *title;
    int width;
    int height;
    PG_Menu *menu;
};

SOEXPORT PG_Window *PG_Window_create(PG_Window *parent, const char *title, int width, int height)
{
    PG_Window *self = malloc(sizeof(PG_Window));
    GCINIT(self);
    privateApi.container.create(self);
    privateApi.control.create(self);
    self->parent = parent;
    self->closed = 0;
    self->closing = PC_Event_create("closing");
    self->dialogShown = PC_Event_create("dialogShown");
    self->title = PC_String_copy(title);
    self->width = width;
    self->height = height;
    self->menu = 0;
    self->dialog = 0;
    self->dialogResult = 0;

    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.window.create) b->backendApi.window.create(self);
    return self;
}

SOEXPORT PG_Window *PG_Window_parent(const PG_Window *self)
{
    return self->parent;
}

SOEXPORT const char *PG_Window_title(const PG_Window *self)
{
    return self->title;
}

SOEXPORT int PG_Window_width(const PG_Window *self)
{
    return self->width;
}

SOEXPORT int PG_Window_height(const PG_Window *self)
{
    return self->height;
}

SOEXPORT PG_Menu *PG_Window_menu(const PG_Window *self)
{
    return self->menu;
}

SOEXPORT void PG_Window_setMenu(PG_Window *self, PG_Menu *menu)
{
    PG_Menu_destroy(self->menu);
    self->menu = menu;

    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.window.setMenu) b->backendApi.window.setMenu(self, menu);
}

SOEXPORT void PG_Window_close(PG_Window *self)
{
    PC_EventArgs args = PC_EventArgs_init(self->closing, self, 0);
    PC_Event_raise(self->closing, &args);
    if (!args.handled)
    {
        const PG_Backend *b = PG_Backend_current();
        if (b->backendApi.window.close) b->backendApi.window.close(self);
        self->closed = 1;
    }
}

SOEXPORT void *PG_Window_showDialog(PG_Window *self)
{
    if (!self->parent) return 0;
    self->dialog = 1;
    PG_Control_show(self);
    PG_Control_disable(self->parent);
    PC_EventArgs args = PC_EventArgs_init(self->dialogShown, self, 0);
    PC_Event_raise(self->dialogShown, &args);
    while (self->dialog) PC_EventLoop_processEvents(-1);
    PG_Control_enable(self->parent);
    PG_Control_hide(self);
    return self->dialogResult;
}

SOEXPORT void PG_Window_closeDialog(PG_Window *self, void *result)
{
    self->dialogResult = result;
    self->dialog = 0;
}

SOEXPORT PC_Event *PG_Window_closingEvent(const PG_Window *self)
{
    return self->closing;
}

SOEXPORT PC_Event *PG_Window_dialogShownEvent(const PG_Window *self)
{
    return self->dialogShown;
}

SOEXPORT void PG_Window_destroy(PG_Window *self)
{
    if (!self) return;
    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.window.destroy) b->backendApi.window.destroy(self);
    PC_Event_destroy(self->dialogShown);
    PC_Event_destroy(self->closing);
    free(self->title);
    privateApi.control.destroy(self);
    privateApi.container.destroy(self);
    free(self);
}

static thread_local PC_Event *lastWindowClosed = 0;

SOEXPORT PC_Event *PG_Window_lastWindowClosedEvent(void)
{
    if (!lastWindowClosed)
    {
        lastWindowClosed = PC_Event_create("lastWindowClosed");
    }
    return lastWindowClosed;
}
