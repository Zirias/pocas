#include <windows.h>
#include <stdint.h>

#include <pocas/core/dso.h>

struct Dso
{
    struct HINSTANCE__ module;
};

SOEXPORT Dso *Dso_load(const char *path)
{
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    Dso *self = (Dso *)LoadLibrary(path);
    SetErrorMode(0);
    return self;
}

SOEXPORT void *Dso_symbol(Dso *self, const char *name)
{
    return (void *)(uintptr_t)GetProcAddress(&(self->module), name);
}

SOEXPORT const char *Dso_string(Dso *self, const char *name)
{
    return *((const char **)Dso_symbol(self, name));
}

SOEXPORT void Dso_close(Dso *self)
{
    FreeLibrary(&(self->module));
}
