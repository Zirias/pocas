#ifndef POCAS_CORE_EVENT_H
#define POCAS_CORE_EVENT_H

#include <stddef.h>

#include <pocas/core/decl.h>

#define EVENT(classname, eventname) \
    Event *classname##_##eventname##Event(const classname *self)

#define EVENT_REG(classname, object, eventname, handler) \
    Event_register(classname##_##eventname##Event(object), handler)

#define EVENT_UNREG(classname, object, eventname, handler) \
    Event_unregister(classname##_##eventname##Event(object), handler)

typedef struct Event Event;
typedef struct EventArgs EventArgs;
typedef struct DataEventData DataEventData;
typedef struct PlatformEvent PlatformEvent;

typedef struct EventHandler EventHandler;
typedef void (*EventHandlerCallback)(void *self, EventArgs *args);

DECLEXPORT Event *Event_create(const char *name);
DECLEXPORT void Event_register(Event *self, const EventHandler *handler);
DECLEXPORT void Event_unregister(Event *self, const EventHandler *handler);
DECLEXPORT void Event_clear(Event *self);
DECLEXPORT void Event_raise(const Event *self, EventArgs *args);
DECLEXPORT const char *Event_name(const Event *self);
DECLEXPORT void Event_registerWithLoop(const Event *self, const PlatformEvent *pe, void *sender);
DECLEXPORT void Event_unregisterWithLoop(const Event *self);
DECLEXPORT void Event_destroy(Event *self);

DECLEXPORT EventArgs *Event_await(int timeout);

DECLEXPORT EventArgs *EventArgs_create(const Event *event, void *sender, void *data);
DECLEXPORT const Event *EventArgs_event(const EventArgs *self);
DECLEXPORT void *EventArgs_sender(const EventArgs *self);
DECLEXPORT void *EventArgs_data(const EventArgs *self);
DECLEXPORT int EventArgs_handled(const EventArgs *self);
DECLEXPORT void EventArgs_setHandled(EventArgs *self);
DECLEXPORT void EventArgs_destroy(EventArgs *self);

DECLEXPORT DataEventData *DataEventData_create(const void *sourceData, size_t dataSize);
DECLEXPORT size_t DataEventData_size(const DataEventData *self);
DECLEXPORT const void * DataEventData_data(const DataEventData *self);
DECLEXPORT void DataEventData_destroy(DataEventData *self);

DECLEXPORT void PlatformEvent_destroy(PlatformEvent *self);

DECLEXPORT EventHandler *EventHandler_create(void *object, EventHandlerCallback callback);
DECLEXPORT void EventHandler_invoke(const EventHandler *self, EventArgs *args);
DECLEXPORT void EventHandler_destroy(EventHandler *self);

#endif
