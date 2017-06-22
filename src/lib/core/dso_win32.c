#include <windows.h>
#include <stdint.h>
#ifdef DEBUG
#include <stdio.h>
#endif

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
#ifdef DEBUG
    if (!self)
    {
        char buf[1024];
        DWORD error = GetLastError();
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, error, 0, buf, 1024, 0);
        fprintf(stderr, "[DEBUG] 0x%lx %s %s", error, path, buf);
    }
#endif
    return self;
}

SOEXPORT void *PC_Dso_symbol(PC_Dso *self, const char *name)
{
    void *symbol = (void *)(uintptr_t)GetProcAddress(&(self->module), name);
#ifdef DEBUG
    if (!symbol)
    {
        char buf[1024];
        DWORD error = GetLastError();
        FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, 0, error, 0, buf, 1024, 0);
        fprintf(stderr, "[DEBUG] 0x%lx %s %s", error, name, buf);
    }
#endif
    return symbol;
}

SOEXPORT const char *PC_Dso_string(PC_Dso *self, const char *name)
{
    const char **sptr = (const char **)PC_Dso_symbol(self, name);
    if (sptr) return *sptr;
    return 0;
}

SOEXPORT void PC_Dso_close(PC_Dso *self)
{
    FreeLibrary(&(self->module));
}
