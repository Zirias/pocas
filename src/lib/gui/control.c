#include <stdlib.h>
#include <string.h>

#include <pocas/core/event.h>

#include <pocas/gui/extents.h>
#include <pocas/gui/bounds.h>
#include <pocas/gui/container.h>
#include "internal.h"
#include "control_internal.h"

typedef struct Control
{
    Bounds bounds;
    ControlDockMode mode;
    ControlAlignment alignment;
    Extents margin;
    Extents padding;
    int shown;
    unsigned int minWidth;
    unsigned int minHeight;
    unsigned int contentWidth;
    unsigned int contentHeight;
    Event *resized;
    Event *shownChanged;
    Event *containerChanged;
    Event *minSizeChanged;
    void *container;
} Control;

SOLOCAL int Control_create(void *self)
{
    Control *c = calloc(1, sizeof(Control));
    if (!privateApi.setControlObject(self, c))
    {
        free(c);
        return 0;
    }
    c->resized = Event_create("resized");
    c->shownChanged = Event_create("shownChanged");
    c->containerChanged = Event_create("containerChanged");
    c->minSizeChanged = Event_create("minSizeChanged");
    return 1;
}

SOEXPORT ControlDockMode Control_dockMode(const void *self)
{
    Control *c = privateApi.controlObject(self);
    return c->mode;
}

SOEXPORT void Control_setDockMode(void *self, ControlDockMode mode)
{
    Control *c = privateApi.controlObject(self);
    c->mode = mode;
}

SOEXPORT ControlAlignment Control_alignment(const void *self)
{
    Control *c = privateApi.controlObject(self);
    return c->alignment;
}

SOEXPORT void Control_setAlignment(void *self, ControlAlignment alignment)
{
    Control *c = privateApi.controlObject(self);
    c->alignment = alignment;
}

SOEXPORT Event *Control_resizedEvent(const void *self)
{
    Control *c = privateApi.controlObject(self);
    return c->resized;
}

SOEXPORT Event *Control_shownChangedEvent(const void *self)
{
    Control *c = privateApi.controlObject(self);
    return c->shownChanged;
}

SOEXPORT Event *Control_containerChangedEvent(const void *self)
{
    Control *c = privateApi.controlObject(self);
    return c->containerChanged;
}

SOEXPORT Event *Control_minSizeChangedEvent(const void *self)
{
    Control *c = privateApi.controlObject(self);
    return c->minSizeChanged;
}

SOEXPORT void Control_bounds(const void *self, Bounds *b)
{
    Control *c = privateApi.controlObject(self);
    memcpy(b, &c->bounds, sizeof(Bounds));
}

SOEXPORT void Control_setBounds(void *self, const Bounds *b)
{
    Control *c = privateApi.controlObject(self);
    memcpy(&c->bounds, b, sizeof(Bounds));
    if (c->container)
    {
        Bounds cb;
        Container_bounds(c->container, &cb);
        Bounds_fitInto(&c->bounds, &cb);
    }
    const Backend *be = Backend_current();
    if (be->backendApi.control.setBounds)
        be->backendApi.control.setBounds(self, b);
    EventArgs *args = EventArgs_create(c->resized, self, &c->bounds);
    Event_raise(c->resized, args);
    EventArgs_destroy(args);
}

SOEXPORT void Control_margin(const void *self, Extents *e)
{
    Control *c = privateApi.controlObject(self);
    memcpy(e, &c->margin, sizeof(Extents));
}

SOEXPORT void Control_setMargin(void *self, const Extents *e)
{
    Control *c = privateApi.controlObject(self);
    memcpy(&c->margin, e, sizeof(Extents));
}

SOEXPORT void Control_padding(const void *self, Extents *e)
{
    Control *c = privateApi.controlObject(self);
    memcpy(e, &c->padding, sizeof(Extents));
}

SOEXPORT void Control_setPadding(void *self, const Extents *e)
{
    Control *c = privateApi.controlObject(self);
    memcpy(&c->padding, e, sizeof(Extents));
}

SOEXPORT unsigned int Control_minWidth(const void *self)
{
    Control *c = privateApi.controlObject(self);
    return c->contentWidth > c->minWidth ? c->contentWidth : c->minWidth;
}

SOEXPORT unsigned int Control_minHeight(const void *self)
{
    Control *c = privateApi.controlObject(self);
    return c->contentHeight > c->minHeight ? c->contentHeight : c->minHeight;
}

static void fireEventIfMinSizeChanged(void *self,
        unsigned int width, unsigned int height)
{
    Control *c = privateApi.controlObject(self);
    Bounds msb;
    msb.x = 0;
    msb.y = 0;
    msb.width = Control_minWidth(self);
    msb.height = Control_minHeight(self);
    if (msb.width != width || msb.height != height)
    {
        EventArgs *args = EventArgs_create(c->minSizeChanged, self, &msb);
        Event_raise(c->minSizeChanged, args);
        EventArgs_destroy(args);
    }
}

SOEXPORT void Control_setMinSize(void *self,
        unsigned int minWidth, unsigned int minHeight)
{
    Control *c = privateApi.controlObject(self);
    unsigned int currentMinWidth = Control_minWidth(self);
    unsigned int currentMinHeight = Control_minHeight(self);
    c->minWidth = minWidth;
    c->minHeight = minHeight;
    fireEventIfMinSizeChanged(self, currentMinWidth, currentMinHeight);
}

SOLOCAL void Control_setContentSize(void *self,
        unsigned int width, unsigned int height)
{
    Control *c = privateApi.controlObject(self);
    unsigned int currentMinWidth = Control_minWidth(self);
    unsigned int currentMinHeight = Control_minHeight(self);
    c->contentWidth = width;
    c->contentHeight = height;
    fireEventIfMinSizeChanged(self, currentMinWidth, currentMinHeight);
}

SOEXPORT int Control_shown(const void *self)
{
    Control *c = privateApi.controlObject(self);
    return c->shown;
}

SOEXPORT void Control_setShown(void *self, int shown)
{
    Control *c = privateApi.controlObject(self);
    if (c->shown == !!shown) return;
    c->shown = !!shown;
    const Backend *be = Backend_current();
    if (be->backendApi.control.setShown)
        be->backendApi.control.setShown(self, c->shown);
    EventArgs *args = EventArgs_create(c->shownChanged, self, (void *)c->shown);
    Event_raise(c->shownChanged, args);
    EventArgs_destroy(args);
}

static void updateBounds(void *self, Bounds *nb)
{
    Control *c = privateApi.controlObject(self);
    memcpy(&c->bounds, nb, sizeof(Bounds));
    c->bounds.x += c->margin.left;
    c->bounds.y += c->margin.top;
    c->bounds.width = c->bounds.width > c->margin.left + c->margin.right ?
                c->bounds.width - c->margin.left - c->margin.right : 0;
    c->bounds.height = c->bounds.height > c->margin.top + c->margin.bottom ?
                c->bounds.height - c->margin.top - c->margin.bottom : 0;
    const Backend *b = Backend_current();
    if (b->backendApi.control.setBounds)
        b->backendApi.control.setBounds(self, &c->bounds);
    EventArgs *args = EventArgs_create(c->resized, self, &c->bounds);
    Event_raise(c->resized, args);
    EventArgs_destroy(args);
}

static void containerResized(void *self, EventArgs *args)
{
    Bounds *nb = EventArgs_evInfo(args);
    updateBounds(self, nb);
}

SOLOCAL void Control_setContainer(void *self, void *container)
{
    Control *c = privateApi.controlObject(self);
    if (container == c->container) return;
    if (c->container)
    {
        Event_unregister(Container_resizedEvent(c->container),
                         self, containerResized);
    }
    c->container = container;
    const Backend *b = Backend_current();
    if (b->backendApi.control.setContainer)
        b->backendApi.control.setContainer(self, container);
    Event_register(Container_resizedEvent(c->container),
                   self, containerResized);
    Bounds cb;
    Container_bounds(container, &cb);
    updateBounds(self, &cb);
    EventArgs *args = EventArgs_create(c->containerChanged, self, container);
    Event_raise(c->containerChanged, args);
    EventArgs_destroy(args);
}

SOLOCAL void *Control_container(const void *self)
{
    Control *c = privateApi.controlObject(self);
    return c->container;
}

SOLOCAL void Control_destroy(void *self)
{
    if (!self) return;
    Control *c = privateApi.controlObject(self);
    Event_destroy(c->minSizeChanged);
    Event_destroy(c->containerChanged);
    Event_destroy(c->shownChanged);
    Event_destroy(c->resized);
    free(c);
}
