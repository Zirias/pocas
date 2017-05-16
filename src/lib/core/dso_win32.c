#include <windows.h>
#include <stdint.h>

#include <pocas/core/dso.h>

struct PC_Dso
{
    struct HINSTANCE__ module;
};

SOEXPORT PC_Dso *PC_Dso_load(const char *path)
{
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);
    PC_Dso *self = (PC_Dso *)LoadLibrary(path);
    SetErrorMode(0);
    return self;
}

SOEXPORT void *PC_Dso_symbol(PC_Dso *self, const char *name)
{
    return (void *)(uintptr_t)GetProcAddress(&(self->module), name);
}

SOEXPORT const char *PC_Dso_string(PC_Dso *self, const char *name)
{
    return *((const char **)PC_Dso_symbol(self, name));
}

SOEXPORT void PC_Dso_close(PC_Dso *self)
{
    FreeLibrary(&(self->module));
}
