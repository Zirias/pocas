#include "c11threads.h"

#include <stdlib.h>

#include "eventloop_internal.h"

typedef struct EventLoop
{
    int running;
    int rc;
} EventLoop;

static thread_local EventLoop loop = { 0, 0 };

SOEXPORT int EventLoop_run(void)
{
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

SOEXPORT int EventLoop_processEvents(int timeout)
{
    return eventProcessor(timeout);
}

SOEXPORT EventProcessor EventLoop_replaceEventProcessor(EventProcessor ep)
{
    EventProcessor oldEventProcessor = eventProcessor;
    eventProcessor = ep;
    return oldEventProcessor;
}
