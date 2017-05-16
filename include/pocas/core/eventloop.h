#ifndef POCAS_CORE_EVENTLOOP_H
#define POCAS_CORE_EVENTLOOP_H

#include <pocas/core/decl.h>

typedef int (*PC_EventProcessor)(int timeout);

DECLEXPORT int PC_EventLoop_run(void);
DECLEXPORT void PC_EventLoop_exit(int exitCode);
DECLEXPORT int PC_EventLoop_processEvents(int timeout);
DECLEXPORT PC_EventProcessor PC_EventLoop_replaceEventProcessor(PC_EventProcessor ep);

#endif
