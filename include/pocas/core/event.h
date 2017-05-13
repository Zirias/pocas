#ifndef POCAS_CORE_EVENT_H
#define POCAS_CORE_EVENT_H

#include <stddef.h>

#include <pocas/core/decl.h>

typedef struct Event Event;

typedef struct EventArgs
{
    Event *const event;
    void *const sender;
    void *const evInfo;
    int handled;
} EventArgs;


typedef struct DataEvInfo
{
    size_t size;
    char data[];
} DataEvInfo;

typedef void (*EventHandler)(void *self, EventArgs *args);

DECLEXPORT Event *Event_create(const char *name);
DECLEXPORT void Event_register(Event *self, void *object, EventHandler handler);
DECLEXPORT void Event_unregister(Event *self, void *object, EventHandler handler);
DECLEXPORT int Event_count(const Event *self);
DECLEXPORT void Event_clear(Event *self);
DECLEXPORT void Event_raise(const Event *self, EventArgs *args);
DECLEXPORT const char *Event_name(const Event *self);
DECLEXPORT void Event_destroy(Event *self);

#define EventArgs_init(event, sender, evInfo) {(event), (sender), (evInfo), 0}

#endif
