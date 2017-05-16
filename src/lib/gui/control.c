#include <stdlib.h>
#include <string.h>

#include <pocas/core/event.h>

#include <pocas/gui/extents.h>
#include <pocas/gui/bounds.h>
#include <pocas/gui/container.h>
#include "internal.h"
#include "control_internal.h"

typedef struct PG_Control
{
    PG_Bounds bounds;
    PG_ControlDockMode mode;
    PG_ControlAlignment alignment;
    PG_Extents margin;
    PG_Extents padding;
    int shown;
    int enabled;
    unsigned int minWidth;
    unsigned int minHeight;
    unsigned int contentWidth;
    unsigned int contentHeight;
    PC_Event *resized;
    PC_Event *shownChanged;
    PC_Event *containerChanged;
    PC_Event *minSizeChanged;
    void *container;
} PG_Control;

SOLOCAL int PG_Control_create(void *self)
{
    PG_Control *c = calloc(1, sizeof(PG_Control));
    if (!privateApi.setControlObject(self, c))
    {
        free(c);
        return 0;
    }
    c->enabled = 1;
    c->resized = PC_Event_create("resized");
    c->shownChanged = PC_Event_create("shownChanged");
    c->containerChanged = PC_Event_create("containerChanged");
    c->minSizeChanged = PC_Event_create("minSizeChanged");
    return 1;
}

SOEXPORT PG_ControlDockMode PG_Control_dockMode(const void *self)
{
    PG_Control *c = privateApi.controlObject(self);
    return c->mode;
}

SOEXPORT void PG_Control_setDockMode(void *self, PG_ControlDockMode mode)
{
    PG_Control *c = privateApi.controlObject(self);
    c->mode = mode;
}

SOEXPORT PG_ControlAlignment PG_Control_alignment(const void *self)
{
    PG_Control *c = privateApi.controlObject(self);
    return c->alignment;
}

SOEXPORT void PG_Control_setAlignment(void *self, PG_ControlAlignment alignment)
{
    PG_Control *c = privateApi.controlObject(self);
    c->alignment = alignment;
}

SOEXPORT PC_Event *PG_Control_resizedEvent(const void *self)
{
    PG_Control *c = privateApi.controlObject(self);
    return c->resized;
}

SOEXPORT PC_Event *PG_Control_shownChangedEvent(const void *self)
{
    PG_Control *c = privateApi.controlObject(self);
    return c->shownChanged;
}

SOEXPORT PC_Event *PG_Control_containerChangedEvent(const void *self)
{
    PG_Control *c = privateApi.controlObject(self);
    return c->containerChanged;
}

SOEXPORT PC_Event *PG_Control_minSizeChangedEvent(const void *self)
{
    PG_Control *c = privateApi.controlObject(self);
    return c->minSizeChanged;
}

SOEXPORT void PG_Control_bounds(const void *self, PG_Bounds *b)
{
    PG_Control *c = privateApi.controlObject(self);
    memcpy(b, &c->bounds, sizeof(PG_Bounds));
}

SOEXPORT void PG_Control_setPG_Bounds(void *self, const PG_Bounds *b)
{
    PG_Control *c = privateApi.controlObject(self);
    memcpy(&c->bounds, b, sizeof(PG_Bounds));
    if (c->container)
    {
        PG_Bounds cb;
        PG_Container_bounds(c->container, &cb);
        PG_Bounds_fitInto(&c->bounds, &cb);
    }
    const PG_Backend *be = PG_Backend_current();
    if (be->backendApi.control.setBounds)
        be->backendApi.control.setBounds(self, b);
    PC_EventArgs args = PC_EventArgs_init(c->resized, self, &c->bounds);
    PC_Event_raise(c->resized, &args);
}

SOEXPORT void PG_Control_margin(const void *self, PG_Extents *e)
{
    PG_Control *c = privateApi.controlObject(self);
    memcpy(e, &c->margin, sizeof(PG_Extents));
}

SOEXPORT void PG_Control_setMargin(void *self, const PG_Extents *e)
{
    PG_Control *c = privateApi.controlObject(self);
    memcpy(&c->margin, e, sizeof(PG_Extents));
}

SOEXPORT void PG_Control_padding(const void *self, PG_Extents *e)
{
    PG_Control *c = privateApi.controlObject(self);
    memcpy(e, &c->padding, sizeof(PG_Extents));
}

static void fireEventIfMinSizeChanged(void *self,
        unsigned int width, unsigned int height)
{
    PG_Control *c = privateApi.controlObject(self);
    PG_Bounds msb;
    msb.x = 0;
    msb.y = 0;
    msb.width = PG_Control_minWidth(self);
    msb.height = PG_Control_minHeight(self);
    if (msb.width != width || msb.height != height)
    {
        PC_EventArgs args = PC_EventArgs_init(c->minSizeChanged, self, &msb);
        PC_Event_raise(c->minSizeChanged, &args);
    }
}

SOEXPORT void PG_Control_setPadding(void *self, const PG_Extents *e)
{
    PG_Control *c = privateApi.controlObject(self);
    unsigned int currentMinWidth = PG_Control_minWidth(self);
    unsigned int currentMinHeight = PG_Control_minHeight(self);
    memcpy(&c->padding, e, sizeof(PG_Extents));
    fireEventIfMinSizeChanged(self, currentMinWidth, currentMinHeight);
}

SOEXPORT unsigned int PG_Control_minWidth(const void *self)
{
    PG_Control *c = privateApi.controlObject(self);
    return c->minWidth ? c->minWidth :
            (c->contentWidth + c->padding.left + c->padding.right);
}

SOEXPORT unsigned int PG_Control_minHeight(const void *self)
{
    PG_Control *c = privateApi.controlObject(self);
    return c->minHeight ? c->minHeight :
            (c->contentHeight + c->padding.top + c->padding.bottom);
}

SOEXPORT void PG_Control_setMinSize(void *self,
        unsigned int minWidth, unsigned int minHeight)
{
    PG_Control *c = privateApi.controlObject(self);
    unsigned int currentMinWidth = PG_Control_minWidth(self);
    unsigned int currentMinHeight = PG_Control_minHeight(self);
    c->minWidth = minWidth;
    c->minHeight = minHeight;
    fireEventIfMinSizeChanged(self, currentMinWidth, currentMinHeight);
}

SOLOCAL void PG_Control_setContentSize(void *self,
        unsigned int width, unsigned int height)
{
    PG_Control *c = privateApi.controlObject(self);
    unsigned int currentMinWidth = PG_Control_minWidth(self);
    unsigned int currentMinHeight = PG_Control_minHeight(self);
    c->contentWidth = width;
    c->contentHeight = height;
    fireEventIfMinSizeChanged(self, currentMinWidth, currentMinHeight);
}

SOEXPORT int PG_Control_shown(const void *self)
{
    PG_Control *c = privateApi.controlObject(self);
    return c->shown;
}

SOEXPORT void PG_Control_setShown(void *self, int shown)
{
    PG_Control *c = privateApi.controlObject(self);
    if (c->shown == !!shown) return;
    c->shown = !!shown;
    const PG_Backend *be = PG_Backend_current();
    if (be->backendApi.control.setShown)
        be->backendApi.control.setShown(self, c->shown);
    PC_EventArgs args = PC_EventArgs_init(c->shownChanged, self, &c->shown);
    PC_Event_raise(c->shownChanged, &args);
}

SOEXPORT int PG_Control_enabled(const void *self)
{
    PG_Control *c = privateApi.controlObject(self);
    return c->enabled;
}

SOEXPORT void PG_Control_setEnabled(void *self, int enabled)
{
    PG_Control *c = privateApi.controlObject(self);
    if (c->enabled == !!enabled) return;
    c->enabled = !!enabled;
    const PG_Backend *be = PG_Backend_current();
    if (be->backendApi.control.setEnabled)
        be->backendApi.control.setEnabled(self, c->enabled);
}

SOEXPORT void PG_Control_focus(void *self)
{
    const PG_Backend *be = PG_Backend_current();
    if (be->backendApi.control.focus)
        be->backendApi.control.focus(self);
}

static void updateBounds(void *self, PG_Bounds *nb)
{
    PG_Control *c = privateApi.controlObject(self);
    memcpy(&c->bounds, nb, sizeof(PG_Bounds));
    c->bounds.x += c->margin.left;
    c->bounds.y += c->margin.top;
    c->bounds.width = c->bounds.width > c->margin.left + c->margin.right ?
                c->bounds.width - c->margin.left - c->margin.right : 0;
    c->bounds.height = c->bounds.height > c->margin.top + c->margin.bottom ?
                c->bounds.height - c->margin.top - c->margin.bottom : 0;
    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.control.setBounds)
        b->backendApi.control.setBounds(self, &c->bounds);
    PC_EventArgs args = PC_EventArgs_init(c->resized, self, &c->bounds);
    PC_Event_raise(c->resized, &args);
}

static void containerResized(void *self, PC_EventArgs *args)
{
    PG_Bounds *nb = args->evInfo;
    updateBounds(self, nb);
}

SOLOCAL void PG_Control_setContainer(void *self, void *container)
{
    PG_Control *c = privateApi.controlObject(self);
    if (container == c->container) return;
    if (c->container)
    {
        PC_Event_unregister(PG_Container_resizedEvent(c->container),
                         self, containerResized);
    }
    c->container = container;
    if (!container) return;
    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.control.setContainer)
        b->backendApi.control.setContainer(self, container);
    PC_Event_register(PG_Container_resizedEvent(c->container),
                   self, containerResized);
    PG_Bounds cb;
    PG_Container_bounds(container, &cb);
    updateBounds(self, &cb);
    PC_EventArgs args = PC_EventArgs_init(c->containerChanged, self, container);
    PC_Event_raise(c->containerChanged, &args);
}

SOLOCAL void *PG_Control_container(const void *self)
{
    PG_Control *c = privateApi.controlObject(self);
    return c->container;
}

SOLOCAL void PG_Control_destroy(void *self)
{
    if (!self) return;
    PG_Control *c = privateApi.controlObject(self);
    PC_Event_destroy(c->minSizeChanged);
    PC_Event_destroy(c->containerChanged);
    PC_Event_destroy(c->shownChanged);
    PC_Event_destroy(c->resized);
    free(c);
}
