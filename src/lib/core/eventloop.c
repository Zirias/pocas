#include "c11threads.h"

#include <stdlib.h>

#include <pocas/core/eventloop.h>

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
