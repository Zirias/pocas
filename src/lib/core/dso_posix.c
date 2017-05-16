#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#include <pocas/core/dso.h>

struct PC_Dso
{
    uintptr_t so;
};

SOEXPORT PC_Dso *PC_Dso_load(const char *path)
{
    PC_Dso *self = dlopen(path, RTLD_NOW);
#ifdef DEBUG
    if (!self) fprintf(stderr, "error loading dso `%s': %s\n",
	    path, dlerror());
#endif
    return self;
}

SOEXPORT void *PC_Dso_symbol(PC_Dso *self, const char *name)
{
    return dlsym(&self->so, name);
}

SOEXPORT const char *PC_Dso_string(PC_Dso *self, const char *name)
{
    return *((const char **) dlsym(&self->so, name));
}

SOEXPORT void PC_Dso_close(PC_Dso *self)
{
    dlclose(&self->so);
}
