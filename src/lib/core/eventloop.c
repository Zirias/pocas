#include "c11threads.h"

#include <stdlib.h>

#include "eventloop_internal.h"

typedef struct PC_EventLoop
{
    int running;
    int rc;
} PC_EventLoop;

static thread_local PC_EventLoop loop = { 0, 0 };

SOEXPORT int PC_EventLoop_run(void)
{
    if (loop.running) return EXIT_FAILURE;
    loop.running = 1;
    while (loop.running)
    {
        PC_EventLoop_processEvents(-1);
    }
    return loop.rc;
}

SOEXPORT void PC_EventLoop_exit(int rc)
{
    if (loop.running)
    {
        loop.running = 0;
        loop.rc = rc;
    }
}

SOEXPORT int PC_EventLoop_processEvents(int timeout)
{
    return eventProcessor(timeout);
}

SOEXPORT PC_EventProcessor PC_EventLoop_replaceEventProcessor(PC_EventProcessor ep)
{
    PC_EventProcessor oldEventProcessor = eventProcessor;
    eventProcessor = ep;
    return oldEventProcessor;
}
