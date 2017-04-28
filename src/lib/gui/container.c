#include <stdlib.h>

#include <pocas/core/event.h>

#include "internal.h"
#include "container_internal.h"

typedef struct Container
{
    unsigned int width;
    unsigned int height;
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
    c->control = control;
    privateApi.control.setContainer(control, self);
}

SOEXPORT unsigned int Container_width(const void *self)
{
    Container *c = privateApi.containerObject(self);
    return c->width;
}

SOLOCAL void Container_setWidth(void *self, unsigned int width)
{
    Container *c = privateApi.containerObject(self);
    c->width = width;
}

SOEXPORT unsigned int Container_height(const void *self)
{
    Container *c = privateApi.containerObject(self);
    return c->height;
}

SOLOCAL void Container_setHeight(void *self, unsigned int height)
{
    Container *c = privateApi.containerObject(self);
    c->height = height;
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
