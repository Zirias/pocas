#include <stdlib.h>
#include <string.h>

#include <pocas/core/event.h>
#include <pocas/core/list.h>

#include "internal.h"
#include <pocas/gui/bounds.h>
#include <pocas/gui/container.h>
#include <pocas/gui/control.h>
#include <pocas/gui/hbox.h>

struct HBox
{
    GuiClass gc;
    List *elements;
    List *controls;
};

static void *hboxElementCreator(HBox *self, void *control)
{
    GuiClass *hbe = malloc(sizeof(GuiClass));
    GCINIT(hbe);
    privateApi.container.create(hbe);
    privateApi.setControlObject(hbe, self);
    Container_setControl(hbe, control);
    return hbe;
}

static void hboxElementDeleter(void *hbe)
{
    Container_setControl(hbe, 0);
    privateApi.container.destroy(hbe);
    free(hbe);
}

static int hboxElementMatcher(const void *element, void *control)
{
    return Container_control(element) == control;
}

static void updateElementsBounds(HBox *self, Bounds *b)
{
    if (!List_length(self->elements)) return;
    unsigned int elementWidth = b->width / List_length(self->elements);
    Bounds eb;
    memcpy(&eb, b, sizeof(Bounds));
    eb.width = elementWidth;
    ListIterator *ei = List_iterator(self->elements);
    while (ListIterator_moveNext(ei))
    {
        GuiClass *hbe = ListIterator_current(ei);
        privateApi.container.setBounds(hbe, &eb);
        eb.x += elementWidth;
    }
    ListIterator_destroy(ei);
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
    Event_register(Control_resizedEvent(self), self, onResized);
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
    Event_unregister(Control_resizedEvent(self), self, onResized);
    List_destroy(self->elements);
    List_destroy(self->controls);
    privateApi.control.destroy(self);
    free(self);
}
