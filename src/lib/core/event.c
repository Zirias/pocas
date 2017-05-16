#include "c11threads.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <pocas/core/list.h>
#include "eventloop_internal.h"

#include <pocas/core/event.h>

struct PC_Event
{
    const char *name;
    PC_List *handlers;
};

struct handlerData
{
    void *object;
    PC_EventHandler handler;
};

SOEXPORT PC_Event *PC_Event_create(const char *name)
{
    PC_Event *self = malloc(sizeof(PC_Event));
    self->name = name;
    self->handlers = PC_List_create(0, free, 0);
    return self;
}

SOEXPORT void PC_Event_register(PC_Event *self, void *object, PC_EventHandler handler)
{
    struct handlerData *data = malloc(sizeof(*data));
    data->object = object;
    data->handler = handler;
    PC_List_append(self->handlers, data);
}

int handlerElementSelector(const void *element, void *args)
{
    const struct handlerData *data = element;
    struct handlerData *match = args;
    return (data->handler == match->handler && data->object == match->object);
}

SOEXPORT void PC_Event_unregister(PC_Event *self, void *object, PC_EventHandler handler)
{
    struct handlerData data = { object, handler };
    PC_List_removeMatching(self->handlers, handlerElementSelector, &data);
}

SOEXPORT int PC_Event_count(const PC_Event *self)
{
    return PC_List_length(self->handlers);
}

SOEXPORT void PC_Event_clear(PC_Event *self)
{
    PC_List_clear(self->handlers);
}

SOEXPORT void PC_Event_raise(const PC_Event *self, PC_EventArgs *args)
{
    PC_ListIterator *i = PC_List_iterator(self->handlers);
    while (PC_ListIterator_moveNext(i))
    {
        const struct handlerData *data = PC_ListIterator_current(i);
        data->handler(data->object, args);
        if (args->handled) break;
    }
    PC_ListIterator_destroy(i);
}

SOEXPORT const char *PC_Event_name(const PC_Event *self)
{
    return self->name;
}

SOEXPORT void PC_Event_destroy(PC_Event *self)
{
    if (!self) return;
    PC_List_destroy(self->handlers);
    free(self);
}

