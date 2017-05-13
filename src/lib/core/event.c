#include "c11threads.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <pocas/core/list.h>
#include <pocas/core/mtwaitqueue.h>
#include "eventloop_internal.h"

#include <pocas/core/event.h>

struct Event
{
    const char *name;
    List *handlers;
};

struct handlerData
{
    void *object;
    EventHandler handler;
};

SOEXPORT Event *Event_create(const char *name)
{
    Event *self = malloc(sizeof(Event));
    self->name = name;
    self->handlers = List_create(0, free, 0);
    return self;
}

SOEXPORT void Event_register(Event *self, void *object, EventHandler handler)
{
    struct handlerData *data = malloc(sizeof(*data));
    data->object = object;
    data->handler = handler;
    List_append(self->handlers, data);
}

int handlerElementSelector(const void *element, void *args)
{
    const struct handlerData *data = element;
    struct handlerData *match = args;
    return (data->handler == match->handler && data->object == match->object);
}

SOEXPORT void Event_unregister(Event *self, void *object, EventHandler handler)
{
    struct handlerData data = { object, handler };
    List_removeMatching(self->handlers, handlerElementSelector, &data);
}

SOEXPORT int Event_count(const Event *self)
{
    return List_length(self->handlers);
}

SOEXPORT void Event_clear(Event *self)
{
    List_clear(self->handlers);
}

SOEXPORT void Event_raise(const Event *self, EventArgs *args)
{
    ListIterator *i = List_iterator(self->handlers);
    while (ListIterator_moveNext(i))
    {
        const struct handlerData *data = ListIterator_current(i);
        data->handler(data->object, args);
        if (args->handled) break;
    }
    ListIterator_destroy(i);
}

SOEXPORT const char *Event_name(const Event *self)
{
    return self->name;
}

SOEXPORT void Event_destroy(Event *self)
{
    if (!self) return;
    List_destroy(self->handlers);
    free(self);
}

