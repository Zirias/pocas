#include <stdlib.h>
#include <string.h>

#include <pocas/core/event.h>

#include <pocas/gui/bounds.h>
#include <pocas/gui/container.h>
#include "internal.h"
#include "control_internal.h"

typedef struct Control
{
    Bounds bounds;
    ControlDockMode mode;
    int shown;
    Event *resized;
    Event *shownChanged;
    void *container;
} Control;

SOLOCAL int Control_create(void *self)
{
    Control *c = calloc(1, sizeof(Control));
    if (!privateApi.setControlObject(self, c))
    {
        free(c);
        return 0;
    }
    c->resized = Event_create("resized");
    c->shownChanged = Event_create("shownChanged");
    return 1;
}

SOEXPORT ControlDockMode Control_dockMode(const void *self)
{
    Control *c = privateApi.controlObject(self);
    return c->mode;
}

SOEXPORT void Control_setDockMode(void *self, ControlDockMode mode)
{
    Control *c = privateApi.controlObject(self);
    c->mode = mode;
}

SOEXPORT Event *Control_resizedEvent(const void *self)
{
    Control *c = privateApi.controlObject(self);
    return c->resized;
}

SOEXPORT Event *Control_shownChangedEvent(const void *self)
{
    Control *c = privateApi.containerObject(self);
    return c->shownChanged;
}

SOEXPORT void Control_bounds(const void *self, Bounds *b)
{
    Control *c = privateApi.controlObject(self);
    memcpy(b, &c->bounds, sizeof(Bounds));
}

SOEXPORT void Control_setBounds(void *self, const Bounds *b)
{
    Control *c = privateApi.controlObject(self);
    memcpy(&c->bounds, b, sizeof(Bounds));
    if (c->container)
    {
        Bounds cb;
        Container_bounds(c->container, &cb);
        Bounds_fitInto(&c->bounds, &cb);
    }
    const Backend *be = Backend_current();
    if (be->backendApi.control.setBounds)
        be->backendApi.control.setBounds(self, b);
    EventArgs *args = EventArgs_create(c->resized, self, &c->bounds);
    Event_raise(c->resized, args);
    EventArgs_destroy(args);
}

SOEXPORT int Control_shown(const void *self)
{
    Control *c = privateApi.controlObject(self);
    return c->shown;
}

SOEXPORT void Control_setShown(void *self, int shown)
{
    Control *c = privateApi.controlObject(self);
    c->shown = !!shown;
    const Backend *be = Backend_current();
    if (be->backendApi.control.setShown)
        be->backendApi.control.setShown(self, c->shown);
}

static void updateBounds(void *self, Bounds *nb)
{
    Control *c = privateApi.controlObject(self);
    memcpy(&c->bounds, nb, sizeof(Bounds));
    const Backend *b = Backend_current();
    if (b->backendApi.control.setBounds)
        b->backendApi.control.setBounds(self, &c->bounds);
    EventArgs *args = EventArgs_create(c->resized, self, &c->bounds);
    Event_raise(c->resized, args);
    EventArgs_destroy(args);
}

static void containerResized(void *self, EventArgs *args)
{
    Bounds *nb = EventArgs_evInfo(args);
    updateBounds(self, nb);
}

SOLOCAL void Control_setContainer(void *self, void *container)
{
    Control *c = privateApi.controlObject(self);
    if (c->container)
    {
        Event_unregister(Container_resizedEvent(c->container),
                         self, containerResized);
    }
    c->container = container;
    const Backend *b = Backend_current();
    if (b->backendApi.control.setContainer)
        b->backendApi.control.setContainer(self, container);
    Event_register(Container_resizedEvent(c->container),
                   self, containerResized);
    Bounds cb;
    Container_bounds(container, &cb);
    updateBounds(self, &cb);
}

SOLOCAL void *Control_container(const void *self)
{
    Control *c = privateApi.controlObject(self);
    return c->container;
}

SOLOCAL void Control_destroy(void *self)
{
    if (!self) return;
    Control *c = privateApi.controlObject(self);
    Event_destroy(c->shownChanged);
    Event_destroy(c->resized);
    free(c);
}
