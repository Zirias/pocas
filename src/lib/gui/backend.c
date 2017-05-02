#include <string.h>

#include <pocas/gui/private/backend.h>

#include "internal.h"
#include "control_internal.h"
#include "container_internal.h"

extern Backend *defaultBackend;
static Backend *currentBackend = 0;

static void *backendObject(const void *frontendObject)
{
    if (!ISGCOBJ(frontendObject)) return 0;
    const GuiClass *gc = frontendObject;
    return gc->backend;
}

static int setBackendObject(void *frontendObject, void *backendObject)
{
    if (!ISGCOBJ(frontendObject)) return 0;
    GuiClass *gc = frontendObject;
    gc->backend = backendObject;
    return 1;
}

static void *controlObject(const void *frontendObject)
{
    if (!ISGCOBJ(frontendObject)) return 0;
    const GuiClass *gc = frontendObject;
    return gc->control;
}

static int setControlObject(void *frontendObject, void *controlObject)
{
    if (!ISGCOBJ(frontendObject)) return 0;
    GuiClass *gc = frontendObject;
    gc->control = controlObject;
    return 1;
}

static void *containerObject(const void *frontendObject)
{
    if (!ISGCOBJ(frontendObject)) return 0;
    const GuiClass *gc = frontendObject;
    return gc->container;
}

static int setContainerObject(void *frontendObject, void *containerObject)
{
    if (!ISGCOBJ(frontendObject)) return 0;
    GuiClass *gc = frontendObject;
    gc->container = containerObject;
    return 1;
}

SOLOCAL const GuiPrivateApi privateApi =
{
    .backendObject = backendObject,
    .setBackendObject = setBackendObject,
    .controlObject = controlObject,
    .setControlObject = setControlObject,
    .containerObject = containerObject,
    .setContainerObject = setContainerObject,

    .control = {
        .create = Control_create,
        .container = Control_container,
        .setContainer = Control_setContainer,
        .destroy = Control_destroy
    },

    .container =
    {
        .create = Container_create,
        .setWidth = Container_setWidth,
        .setHeight = Container_setHeight,
        .destroy = Container_destroy
    }
};

SOEXPORT const Backend *Backend_current()
{
    if (!currentBackend)
    {
        currentBackend = defaultBackend;
        currentBackend->privateApi = &privateApi;
    }
    return currentBackend;
}

SOEXPORT void Backend_setCurrent(Backend *backend)
{
    backend->privateApi = &privateApi;
    currentBackend = backend;
}
