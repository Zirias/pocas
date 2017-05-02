#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "control_internal.h"

typedef struct Control
{
    Bounds bounds;
    ControlDockMode mode;
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

SOEXPORT void Control_bounds(const void *self, Bounds *b)
{
    Control *c = privateApi.controlObject(self);
    memcpy(b, &c->bounds, sizeof(Bounds));
}

SOEXPORT void Control_setBounds(void *self, const Bounds *b)
{
    Control *c = privateApi.controlObject(self);

}

SOLOCAL void Control_setContainer(void *self, void *container)
{
    Control *c = privateApi.controlObject(self);
    c->container = container;
    Backend *b = Backend_current();
    if (b->backendApi.control.setContainer)
        b->backendApi.control.setContainer(self, container);
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
    free(c);
}
