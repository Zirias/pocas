#ifndef POCAS_CORE_PROCESSHOST_H
#define POCAS_CORE_PROCESSHOST_H

#include <pocas/core/decl.h>

C_CLASS_DECL(PC_Event);
C_CLASS_DECL(PC_List);

C_CLASS_DECL(PC_ProcessHost);

enum PC_ProcessHostFlags
{
    PC_PH_NONE = 0,
    PC_PH_REDIR_STDIN = 1 << 0,
    PC_PH_REDIR_STDOUT = 1 << 1,
    PC_PH_REDIR_STDERR = 1 << 2,
    PC_PH_REDIR_STDIO = PC_PH_REDIR_STDIN | PC_PH_REDIR_STDOUT | PC_PH_REDIR_STDERR,
    PC_PH_REDIR_STDERR_IS_STDOUT = 1 << 3,
    PC_PH_NO_STDIN = 1 << 4,
    PC_PH_NO_STDOUT = 1 << 5,
    PC_PH_NO_STDERR = 1 << 6,
    PC_PH_NO_STDIO = PC_PH_NO_STDIN | PC_PH_NO_STDOUT | PC_PH_NO_STDERR
};
C_ENUM_DECL(PC_ProcessHostFlags);

DECLEXPORT PC_ProcessHost *PC_ProcessHost_create(const char *executable);
DECLEXPORT int PC_ProcessHost_start(PC_ProcessHost *self, PC_ProcessHostFlags flags, ...);
DECLEXPORT int PC_ProcessHost_startv(PC_ProcessHost *self, PC_ProcessHostFlags flags, const PC_List *args);

DECLEXPORT PC_Event *ProcessHost_stdoutDataEvent(const PC_ProcessHost *self);
DECLEXPORT PC_Event *ProcessHost_stderrDataEvent(const PC_ProcessHost *self);

DECLEXPORT void PC_ProcessHost_destroy(PC_ProcessHost *self);

#endif
