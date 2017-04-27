#ifndef POCAS_CORE_EVENT_H
#define POCAS_CORE_EVENT_H

#include <stddef.h>

#include <pocas/core/decl.h>

typedef struct Event Event;
typedef struct EventArgs EventArgs;

typedef struct DataEvInfo
{
    size_t size;
    char data[];
} DataEvInfo;

typedef void (*EventHandler)(void *self, EventArgs *args);

DECLEXPORT Event *Event_create(const char *name);
DECLEXPORT void Event_register(Event *self, void *object, EventHandler handler);
DECLEXPORT void Event_unregister(Event *self, void *object, EventHandler handler);
DECLEXPORT void Event_clear(Event *self);
DECLEXPORT void Event_raise(const Event *self, EventArgs *args);
DECLEXPORT const char *Event_name(const Event *self);
DECLEXPORT void Event_destroy(Event *self);

DECLEXPORT EventArgs *EventArgs_create(const Event *event, void *sender, void *evInfo);
DECLEXPORT const Event *EventArgs_event(const EventArgs *self);
DECLEXPORT void *EventArgs_sender(const EventArgs *self);
DECLEXPORT void *EventArgs_evInfo(const EventArgs *self);
DECLEXPORT int EventArgs_handled(const EventArgs *self);
DECLEXPORT void EventArgs_setHandled(EventArgs *self);
DECLEXPORT void EventArgs_destroy(EventArgs *self);

#endif
