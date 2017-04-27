#include "backend_internal.h"

extern const Backend *defaultBackend;
static const Backend *currentBackend = 0;

SOEXPORT const Backend *Backend_current()
{
    if (!currentBackend) currentBackend = defaultBackend;
    return currentBackend;
}

SOEXPORT void Backend_setCurrent(const Backend *backend)
{
    currentBackend = backend;
}
