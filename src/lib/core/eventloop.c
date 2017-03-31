#include <stdlib.h>
#include <string.h>
#include <pocas/core/list.h>
#include <pocas/core/event.h>
#include "eventloop_internal.h"
#include "c11threads.h"

typedef struct EventLoopEntry
{
    const Event *ev;
    const PlatformEvent *pe;
    void *sender;
} EventLoopEntry;

typedef struct EventLoop
{
    int running;
    int rc;
    List *entries;
} EventLoop;

static thread_local EventLoop loop = { 0, 0, 0 };

static inline void init()
{
    if (!loop.entries)
    {
        loop.entries = List_create(0,
                (ListElementDeleter)EventLoopEntry_destroy,
                (ListElementCloner)EventLoopEntry_clone);
    }
}

SOLOCAL EventLoopEntry *EventLoopEntry_create(const Event *ev, const PlatformEvent *pe, void *sender)
{
    EventLoopEntry *self = malloc(sizeof(EventLoopEntry));
    self->ev = ev;
    self->pe = pe;
    self->sender = sender;
    return self;
}

SOLOCAL EventLoopEntry *EventLoopEntry_clone(const EventLoopEntry *entry)
{
    void *result = malloc(sizeof(EventLoopEntry));
    memcpy(result, entry, sizeof(EventLoopEntry));
    return result;
}

SOLOCAL const Event *EventLoopEntry_event(const EventLoopEntry *self)
{
    return self->ev;
}

SOLOCAL const PlatformEvent *EventLoopEntry_platformEvent(const EventLoopEntry *self)
{
    return self->pe;
}

SOLOCAL void *EventLoopEntry_sender(const EventLoopEntry *self)
{
    return self->sender;
}

SOLOCAL void EventLoopEntry_destroy(EventLoopEntry *self)
{
    free(self);
}

SOLOCAL List *EventLoop_entries(void)
{
    init();
    return loop.entries;
}

SOEXPORT int EventLoop_run(void)
{
    init();
    if (!List_length(loop.entries)) return EXIT_FAILURE;
    if (loop.running) return EXIT_FAILURE;
    loop.running = 1;
    while (loop.running)
    {
        EventLoop_processEvents(-1);
    }
    return loop.rc;
}

SOEXPORT void EventLoop_exit(int rc)
{
    if (loop.running)
    {
        loop.running = 0;
        loop.rc = rc;
    }
}
