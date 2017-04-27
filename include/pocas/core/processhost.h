#ifndef POCAS_CORE_PROCESSHOST_H
#define POCAS_CORE_PROCESSHOST_H

#include <pocas/core/decl.h>

typedef struct Event Event;
typedef struct List List;

typedef struct ProcessHost ProcessHost;

typedef enum ProcessHostFlags
{
    PH_NONE = 0,
    PH_REDIR_STDIN = 1 << 0,
    PH_REDIR_STDOUT = 1 << 1,
    PH_REDIR_STDERR = 1 << 2,
    PH_REDIR_STDIO = PH_REDIR_STDIN | PH_REDIR_STDOUT | PH_REDIR_STDERR,
    PH_REDIR_STDERR_IS_STDOUT = 1 << 3,
    PH_NO_STDIN = 1 << 4,
    PH_NO_STDOUT = 1 << 5,
    PH_NO_STDERR = 1 << 6,
    PH_NO_STDIO = PH_NO_STDIN | PH_NO_STDOUT | PH_NO_STDERR
} ProcessHostFlags;

DECLEXPORT ProcessHost *ProcessHost_create(const char *executable);
DECLEXPORT int ProcessHost_start(ProcessHost *self, ProcessHostFlags flags, ...);
DECLEXPORT int ProcessHost_startv(ProcessHost *self, ProcessHostFlags flags, const List *args);

DECLEXPORT Event *ProcessHost_stdoutDataEvent(const ProcessHost *self);
DECLEXPORT Event *ProcessHost_stderrDataEvent(const ProcessHost *self);

DECLEXPORT void ProcessHost_destroy(ProcessHost *self);

#endif
