#include <stdlib.h>
#include <dlfcn.h>

#include <pocas/core/dso.h>

struct Dso
{
    uintptr_t so;
};

SOEXPORT Dso *Dso_load(const char *path)
{
    return (Dso *)dlopen(name, RTLD_NOW);
}

SOEXPORT void *Dso_symbol(Dso *self, const char *name)
{
    return dlsym(&self->so, name);
}

SOEXPORT const char *Dso_string(Dso *self, const char *name)
{
    return dlsym(&self->so, name);
}

SOEXPORT void Dso_close(Dso *self)
{
    dlclose(&self->so);
}
