#ifndef POCAS_CORE_DSO_H
#define POCAS_CORE_DSO_H

#include <pocas/core/decl.h>

C_CLASS_DECL(PC_Dso);

DECLEXPORT PC_Dso *PC_Dso_load(const char *path);
DECLEXPORT void *PC_Dso_symbol(PC_Dso *self, const char *name);
DECLEXPORT const char *PC_Dso_string(PC_Dso *self, const char *name);
DECLEXPORT void PC_Dso_close(PC_Dso *self);

#endif
