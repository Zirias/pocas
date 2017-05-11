#include <stdlib.h>
#include <string.h>

#include <pocas/core/event.h>

#include <pocas/gui/bounds.h>
#include "internal.h"
#include "container_internal.h"

typedef struct Container
{
    Bounds b;
    Event *resized;
    void *control;
} Container;

SOLOCAL int Container_create(void *self)
{
    Container *c = calloc(1, sizeof(Container));
    if (!privateApi.setContainerObject(self, c))
    {
        free(c);
        return 0;
    }
    c->resized = Event_create("resized");
    return 1;
}

SOEXPORT void *Container_control(const void *self)
{
    Container *c = privateApi.containerObject(self);
    return c->control;
}

SOEXPORT void Container_setControl(void *self, void *control)
{
    Container *c = privateApi.containerObject(self);
    if (c->control) privateApi.control.setContainer(c->control, 0);
    c->control = control;
    if (control) privateApi.control.setContainer(control, self);
}

SOEXPORT void Container_bounds(const void *self, Bounds *b)
{
    Container *c = privateApi.containerObject(self);
    memcpy(b, &c->b, sizeof(Bounds));
}

SOLOCAL int Container_setBounds(void *self, Bounds *b)
{
    Container *c = privateApi.containerObject(self);
    if (!Bounds_equals(&c->b, b))
    {
        memcpy(&c->b, b, sizeof(Bounds));
        EventArgs *args = EventArgs_create(c->resized, self, &c->b);
        Event_raise(c->resized, args);
        EventArgs_destroy(args);
        return 1;
    }
    return 0;
}

SOEXPORT Event *Container_resizedEvent(const void *self)
{
    Container *c = privateApi.containerObject(self);
    return c->resized;
}

SOLOCAL void Container_destroy(void *self)
{
    if (!self) return;
    Container *c = privateApi.containerObject(self);
    Event_destroy(c->resized);
    free(c);
}
