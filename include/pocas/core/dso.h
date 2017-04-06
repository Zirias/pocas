#ifndef POCAS_CORE_DSO_H
#define POCAS_CORE_DSO_H

#include <pocas/core/decl.h>

typedef struct Dso Dso;

DECLEXPORT Dso *Dso_load(const char *path);
DECLEXPORT void *Dso_symbol(Dso *self, const char *name);
DECLEXPORT const char *Dso_string(Dso *self, const char *name);
DECLEXPORT void Dso_close(Dso *self);

#endif
