#include <string.h>

#include <pocas/gui/private/backend.h>

#include <pocas/gui/window.h>
#include <pocas/gui/menu.h>
#include <pocas/gui/label.h>
#include <pocas/gui/button.h>

#include "internal.h"
#include "control_internal.h"
#include "container_internal.h"

#ifdef DEFAULT_GUI_BACKEND
#define BACKENDPTR(x) x ## Backend
#define BACKENDPTRX(x) BACKENDPTR(x)
#define DEFAULTBACKEND BACKENDPTRX(DEFAULT_GUI_BACKEND)
extern Backend *DEFAULTBACKEND;
static Backend *currentBackend = 0;

#else
static Backend backend_null = {
    .backendApi = {
        .name = 0,
        .control = {
            .setContainer = 0,
            .setBounds = 0,
            .setShown = 0,
        .setEnabled = 0,
        .focus = 0,
        },
        .window = {
            .create = 0,
            .setMenu = 0,
            .close = 0,
            .destroy = 0,
        },
        .menu = {
            .create = 0,
            .addItem = 0,
        .removeItem = 0,
            .destroy = 0,
        },
        .menuItem = {
            .create = 0,
            .destroy = 0,
        },
        .messageBox = {
            .show = 0,
        },
        .label = {
            .create = 0,
            .setText = 0,
            .destroy = 0,
        },
        .button = {
            .create = 0,
            .setText = 0,
            .destroy = 0,
        },
        .textBox = {
            .create = 0,
            .setText = 0,
            .destroy = 0,
        },
    },
    .privateApi = 0,
};
static Backend *currentBackend = &backend_null;
#endif

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
        .bounds = Control_bounds,
        .shown = Control_shown,
        .setContainer = Control_setContainer,
        .setContentSize = Control_setContentSize,
        .destroy = Control_destroy
    },

    .container =
    {
        .create = Container_create,
        .setBounds = Container_setBounds,
        .destroy = Container_destroy
    },

    .window =
    {
        .close = Window_close,
        .title = Window_title,
        .parent = Window_parent,
        .width = Window_width,
        .height = Window_height,
        .lastWindowClosedEvent = Window_lastWindowClosedEvent
    },

    .menuItem =
    {
        .subMenu = MenuItem_subMenu,
        .text = MenuItem_text,
        .select = MenuItem_select
    },

    .label =
    {
        .text = Label_text
    },

    .button =
    {
        .text = Button_text,
        .style = Button_style,
        .click = Button_click
    }
};

SOEXPORT const Backend *Backend_current()
{
#ifdef DEFAULTBACKEND
    if (!currentBackend)
    {
        currentBackend = DEFAULTBACKEND;
    }
#endif
    if (!currentBackend->privateApi)
    {
        currentBackend->privateApi = &privateApi;
    }
    return currentBackend;
}

SOEXPORT void Backend_setCurrent(Backend *backend)
{
    currentBackend = backend;
}
