#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include <pocas/core/dso.h>

struct Dso
{
    uintptr_t so;
};

SOEXPORT Dso *Dso_load(const char *path)
{
    Dso *self = dlopen(path, RTLD_NOW);
#ifdef DEBUG
    if (!self) fprintf(stderr, "error loading dso `%s': %s\n",
	    path, dlerror());
#endif
    return self;
}

SOEXPORT void *Dso_symbol(Dso *self, const char *name)
{
    return dlsym(&self->so, name);
}

SOEXPORT const char *Dso_string(Dso *self, const char *name)
{
    return *((const char **) dlsym(&self->so, name));
}

SOEXPORT void Dso_close(Dso *self)
{
    dlclose(&self->so);
}
