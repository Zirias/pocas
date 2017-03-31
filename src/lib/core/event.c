#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <pocas/core/list.h>
#include "eventloop_internal.h"

#include <pocas/core/event.h>

struct Event
{
    const char *name;
    List *handlers;
};

struct EventArgs
{
    const Event *event;
    void *sender;
    int handled;
    void *data;
};

struct DataEventData
{
    size_t size;
    char data[];
};

struct EventHandler
{
    void *object;
    EventHandlerCallback callback;
};

SOEXPORT Event *Event_create(const char *name)
{
    Event *self = malloc(sizeof(Event));
    self->name = name;
    self->handlers = List_create(0, 0, 0);
    return self;
}

SOEXPORT void Event_register(Event *self, const EventHandler *handler)
{
    List_append(self->handlers, (void *)handler);
}

SOEXPORT void Event_unregister(Event *self, const EventHandler *handler)
{
    List_remove(self->handlers, (void *)handler);
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
        const EventHandler *handler = ListIterator_current(i);
        EventHandler_invoke(handler, args);
        if (EventArgs_handled(args)) break;
    }
    ListIterator_destroy(i);
}

SOEXPORT const char *Event_name(const Event *self)
{
    return self->name;
}

SOEXPORT void Event_registerWithLoop(const Event *self, const PlatformEvent *pe, void *sender)
{
    List_append(EventLoop_entries(), EventLoopEntry_create(self, pe, sender));
}

static int eventMatchesEntry(const void *item, void *arg)
{
    const EventLoopEntry *entry = item;
    const Event *event = arg;
    return EventLoopEntry_event(entry) == event;
}

SOEXPORT void Event_unregisterWithLoop(const Event *self)
{
    List_removeMatching(EventLoop_entries(), eventMatchesEntry, (void *)self);
}

SOEXPORT void Event_destroy(Event *self)
{
    List_destroy(self->handlers);
    free(self);
}

SOEXPORT EventArgs *EventArgs_create(
        const Event *event, void *sender, void *data)
{
    EventArgs *self = malloc(sizeof(EventArgs));
    self->event = event;
    self->sender = sender;
    self->data = data;
    self->handled = 0;
    return self;
}

SOEXPORT const Event *EventArgs_event(const EventArgs *self)
{
    return self->event;
}

SOEXPORT void *EventArgs_sender(const EventArgs *self)
{
    return self->sender;
}

SOEXPORT void *EventArgs_data(const EventArgs *self)
{
    return self->data;
}

SOEXPORT int EventArgs_handled(const EventArgs *self)
{
    return self->handled;
}

SOEXPORT void EventArgs_setHandled(EventArgs *self)
{
    self->handled = 1;
}

SOEXPORT void EventArgs_destroy(EventArgs *self)
{
    free(self);
}

SOEXPORT DataEventData *DataEventData_create(const void *sourceData, size_t dataSize)
{
    DataEventData *self = malloc(sizeof(DataEventData) + dataSize);
    self->size = dataSize;
    if (sourceData) memcpy(self->data, sourceData, dataSize);
    return self;
}

SOEXPORT size_t DataEventData_size(const DataEventData *self)
{
    return self->size;
}

SOEXPORT const void *DataEventData_data(const DataEventData *self)
{
    return self->data;
}

SOEXPORT void DataEventData_destroy(DataEventData *self)
{
    free(self);
}

SOEXPORT EventHandler *EventHandler_create(void *object, EventHandlerCallback callback)
{
    EventHandler *self = malloc(sizeof(EventHandler));
    self->object = object;
    self->callback = callback;
    return self;
}

SOEXPORT void EventHandler_invoke(const EventHandler *self, EventArgs *args)
{
    self->callback(self->object, args);
}

SOEXPORT void EventHandler_destroy(EventHandler *self)
{
    free(self);
}
