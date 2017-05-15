#ifndef POCAS_CORE_EVENTLOOP_H
#define POCAS_CORE_EVENTLOOP_H

#include <pocas/core/decl.h>

typedef int (*EventProcessor)(int timeout);

DECLEXPORT int EventLoop_run(void);
DECLEXPORT void EventLoop_exit(int exitCode);
DECLEXPORT int EventLoop_processEvents(int timeout);
DECLEXPORT EventProcessor EventLoop_replaceEventProcessor(EventProcessor ep);

#endif
