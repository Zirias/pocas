#ifndef POCAS_CORE_EVENTLOOP_H
#define POCAS_CORE_EVENTLOOP_H

#include <pocas/core/decl.h>

DECLEXPORT int EventLoop_run(void);
DECLEXPORT void EventLoop_exit(int rc);
DECLEXPORT int EventLoop_processEvents(int timeout);

#endif
