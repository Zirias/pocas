#ifndef EVENTLOOP_INTERNAL_H
#define EVENTLOOP_INTERNAL_H

#include <pocas/core/eventloop.h>

typedef struct List List;
typedef struct Event Event;
typedef struct PlatformEvent PlatformEvent;
typedef struct EventLoopEntry EventLoopEntry;

List *EventLoop_entries(void);

EventLoopEntry *EventLoopEntry_create(const Event *ev, const PlatformEvent *pe, void *sender);
EventLoopEntry *EventLoopEntry_clone(const EventLoopEntry *self);
const Event *EventLoopEntry_event(const EventLoopEntry *self);
const PlatformEvent *EventLoopEntry_platformEvent(const EventLoopEntry *self);
void *EventLoopEntry_sender(const EventLoopEntry *self);
void EventLoopEntry_destroy(EventLoopEntry *self);

#endif
