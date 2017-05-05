#include <stdlib.h>
#include <string.h>

#include <pocas/core/event.h>
#include <pocas/core/list.h>

#include "internal.h"
#include <pocas/gui/bounds.h>
#include <pocas/gui/container.h>
#include <pocas/gui/control.h>
#include <pocas/gui/extents.h>
#include <pocas/gui/hbox.h>

struct HBox
{
    GuiClass gc;
    List *elements;
    List *controls;
};

static void updateElementsBounds(HBox *self, Bounds *b)
{
    if (!List_length(self->elements)) return;
    Bounds eb;
    eb.x = b->x;
    eb.y = b->y;
    Extents em;
    ListIterator *ei = List_iterator(self->elements);
    while (ListIterator_moveNext(ei))
    {
        GuiClass *hbe = ListIterator_current(ei);
        void *control = Container_control(hbe);
        Control_margin(control, &em);
        eb.width = Control_minWidth(control) + em.left + em.right;
        if (eb.x + eb.width > b->x + b->width)
        {
            if (eb.x >= b->x + b->width) eb.width = 0;
            else eb.width = b->x + b->width - eb.x;
        }
        eb.height = Control_minHeight(control)+ em.top + em.bottom;
        if (eb.y + eb.height > b->y + b->height)
        {
            if (eb.y >= b->y + b->height) eb.height = 0;
            else eb.height = b->y + b->height - eb.y;
        }
        privateApi.container.setBounds(hbe, &eb);
        eb.x += eb.width;
    }
    ListIterator_destroy(ei);
}

static void onElementMinSizeChanged(void *selfPtr, EventArgs *args)
{
    (void)args;
    HBox *self = selfPtr;
    Bounds b;
    Control_bounds(self, &b);
    updateElementsBounds(self, &b);
}

static void *hboxElementCreator(HBox *self, void *control)
{
    GuiClass *hbe = malloc(sizeof(GuiClass));
    GCINIT(hbe);
    privateApi.container.create(hbe);
    privateApi.setControlObject(hbe, self);
    Container_setControl(hbe, control);
    Event_register(Control_minSizeChangedEvent(control),
            self, onElementMinSizeChanged);
    return hbe;
}

static void hboxElementDeleter(void *hbe)
{
    void *control = Container_control(hbe);
    Event_unregister(Control_minSizeChangedEvent(control),
            privateApi.controlObject(hbe), onElementMinSizeChanged);
    Container_setControl(hbe, 0);
    privateApi.container.destroy(hbe);
    free(hbe);
}

static int hboxElementMatcher(const void *element, void *control)
{
    return Container_control(element) == control;
}

static void onContainerChanged(void *selfPtr, EventArgs *args)
{
    const Backend *be = Backend_current();
    if (be->backendApi.control.setContainer)
    {
        HBox *self = selfPtr;
        ListIterator *ci = List_iterator(self->controls);
        while (ListIterator_moveNext(ci))
        {
            void *control = ListIterator_current(ci);
            be->backendApi.control.setContainer(control,
                    EventArgs_evInfo(args));
        }
        ListIterator_destroy(ci);
    }
}

static void onResized(void *selfPtr, EventArgs *args)
{
    HBox *self = selfPtr;
    Bounds *b = EventArgs_evInfo(args);
    updateElementsBounds(self, b);
}

SOEXPORT HBox *HBox_create(void)
{
    HBox *self = malloc(sizeof(HBox));
    GCINIT(self);
    privateApi.control.create(self);
    self->elements = List_create(0, hboxElementDeleter, 0);
    self->controls = List_create(0, 0, 0);
    Event_register(Control_containerChangedEvent(self),
            self, onContainerChanged);
    Event_register(Control_resizedEvent(self),
            self, onResized);
    return self;
}

SOEXPORT ListIterator *HBox_controls(const HBox *self)
{
    return List_iterator(self->controls);
}

SOEXPORT void HBox_removeControl(HBox *self, void *control)
{
    List_removeMatching(self->elements, hboxElementMatcher, control);
    List_remove(self->controls, control);
    Bounds b;
    Control_bounds(self, &b);
    updateElementsBounds(self, &b);
}

SOEXPORT void HBox_addControl(HBox *self, void *control)
{
    List_removeMatching(self->elements, hboxElementMatcher, control);
    List_remove(self->controls, control);
    List_append(self->controls, control);
    List_append(self->elements, hboxElementCreator(self, control));
    Bounds b;
    Control_bounds(self, &b);
    updateElementsBounds(self, &b);
}

SOEXPORT void HBox_destroy(HBox *self)
{
    if (!self) return;
    Event_unregister(Control_resizedEvent(self),
            self, onResized);
    Event_unregister(Control_containerChangedEvent(self),
            self, onContainerChanged);
    List_destroy(self->elements);
    List_destroy(self->controls);
    privateApi.control.destroy(self);
    free(self);
}
