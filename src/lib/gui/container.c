#include <stdlib.h>
#include <string.h>

#include <pocas/core/event.h>

#include <pocas/gui/bounds.h>
#include "internal.h"
#include "container_internal.h"

typedef struct PG_Container
{
    PG_Bounds b;
    PC_Event *resized;
    void *control;
} PG_Container;

SOLOCAL int PG_Container_create(void *self)
{
    PG_Container *c = calloc(1, sizeof(PG_Container));
    if (!privateApi.setContainerObject(self, c))
    {
        free(c);
        return 0;
    }
    c->resized = PC_Event_create("resized");
    return 1;
}

SOEXPORT void *PG_Container_control(const void *self)
{
    PG_Container *c = privateApi.containerObject(self);
    return c->control;
}

SOEXPORT void PG_Container_setControl(void *self, void *control)
{
    PG_Container *c = privateApi.containerObject(self);
    if (c->control) privateApi.control.setContainer(c->control, 0);
    c->control = control;
    if (control) privateApi.control.setContainer(control, self);
}

SOEXPORT void PG_Container_bounds(const void *self, PG_Bounds *b)
{
    PG_Container *c = privateApi.containerObject(self);
    memcpy(b, &c->b, sizeof(PG_Bounds));
}

SOLOCAL int PG_Container_setBounds(void *self, PG_Bounds *b)
{
    PG_Container *c = privateApi.containerObject(self);
    if (!PG_Bounds_equals(&c->b, b))
    {
        memcpy(&c->b, b, sizeof(PG_Bounds));
        PC_EventArgs args = PC_EventArgs_init(c->resized, self, &c->b);
        PC_Event_raise(c->resized, &args);
        return 1;
    }
    return 0;
}

SOEXPORT PC_Event *PG_Container_resizedEvent(const void *self)
{
    PG_Container *c = privateApi.containerObject(self);
    return c->resized;
}

SOLOCAL void PG_Container_destroy(void *self)
{
    if (!self) return;
    PG_Container *c = privateApi.containerObject(self);
    PC_Event_destroy(c->resized);
    free(c);
}
