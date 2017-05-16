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
#define BACKENDPTR(x) PG_ ## x ## Backend
#define BACKENDPTRX(x) BACKENDPTR(x)
#define DEFAULTBACKEND BACKENDPTRX(DEFAULT_GUI_BACKEND)
extern PG_Backend *DEFAULTBACKEND;
#endif
static PG_Backend *currentBackend = 0;

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

SOLOCAL const PG_PrivateApi privateApi =
{
    .backendObject = backendObject,
    .setBackendObject = setBackendObject,
    .controlObject = controlObject,
    .setControlObject = setControlObject,
    .containerObject = containerObject,
    .setContainerObject = setContainerObject,

    .control = {
        .create = PG_Control_create,
        .container = PG_Control_container,
        .bounds = PG_Control_bounds,
        .shown = PG_Control_shown,
        .setContainer = PG_Control_setContainer,
        .setContentSize = PG_Control_setContentSize,
        .destroy = PG_Control_destroy
    },

    .container =
    {
        .create = PG_Container_create,
        .setBounds = PG_Container_setBounds,
        .destroy = PG_Container_destroy
    },

    .window =
    {
        .close = PG_Window_close,
        .title = PG_Window_title,
        .parent = PG_Window_parent,
        .width = PG_Window_width,
        .height = PG_Window_height,
        .lastWindowClosedEvent = PG_Window_lastWindowClosedEvent
    },

    .menuItem =
    {
        .subMenu = PG_MenuItem_subMenu,
        .text = PG_MenuItem_text,
        .select = PG_MenuItem_select
    },

    .label =
    {
        .text = PG_Label_text
    },

    .button =
    {
        .text = PG_Button_text,
        .style = PG_Button_style,
        .click = PG_Button_click
    }
};

SOEXPORT const PG_Backend *PG_Backend_current()
{
    if (!currentBackend)
    {
#ifdef DEFAULTBACKEND
        currentBackend = DEFAULTBACKEND;
#else
        return 0;
#endif
    }
    if (!currentBackend->privateApi)
    {
        currentBackend->privateApi = &privateApi;
    }
    return currentBackend;
}

SOEXPORT void PG_Backend_setCurrent(PG_Backend *backend)
{
    currentBackend = backend;
}
