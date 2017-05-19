#include <stdlib.h>
#include <string.h>

#include <pocas/core/event.h>
#include <pocas/core/list.h>

#include "internal.h"
#include <pocas/gui/bounds.h>
#include <pocas/gui/container.h>
#include <pocas/gui/control.h>
#include <pocas/gui/extents.h>
#include <pocas/gui/boxlayout.h>

struct PG_BoxLayout
{
    GuiClass gc;
    PG_BoxOrientation orientation;
    int expand;
    PC_List *elements;
    PC_List *controls;
};

typedef struct PG_BoxElement
{
    GuiClass gc;
    PG_BoxLayout *box;
} PG_BoxElement;

static void updateElementsBounds(PG_BoxLayout *self, PG_Bounds *b)
{
    if (!PC_List_length(self->elements)) return;
    PG_Bounds eb;
    eb.x = b->x;
    eb.y = b->y;
    PG_Extents em;
    PC_ListIterator *ei = PC_List_iterator(self->elements);
    while (PC_ListIterator_moveNext(ei))
    {
        GuiClass *lbe = PC_ListIterator_current(ei);
        void *control = PG_Container_control(lbe);
        PG_Control_margin(control, &em);
        if (self->orientation == PG_BO_Vertical && self->expand)
        {
            eb.width = PG_Control_minWidth(self);
        }
        else
        {
            eb.width = PG_Control_minWidth(control) + em.left + em.right;
        }
        if (eb.x + eb.width > b->x + b->width)
        {
            if (eb.x >= b->x + b->width) eb.width = 0;
            else eb.width = b->x + b->width - eb.x;
        }
        if (self->orientation == PG_BO_Horizontal && self->expand)
        {
            eb.height = PG_Control_minHeight(self);
        }
        else
        {
            eb.height = PG_Control_minHeight(control) + em.top + em.bottom;
        }
        if (eb.y + eb.height > b->y + b->height)
        {
            if (eb.y >= b->y + b->height) eb.height = 0;
            else eb.height = b->y + b->height - eb.y;
        }
        privateApi.container.setBounds(lbe, &eb);
        if (self->orientation == PG_BO_Horizontal)
        {
            eb.x += eb.width;
        }
        else
        {
            eb.y += eb.height;
        }
    }
    PC_ListIterator_destroy(ei);
}

static void updateContentSize(PG_BoxLayout *self)
{
    unsigned int minWidth = 0;
    unsigned int minHeight = 0;
    PC_ListIterator *ci = PC_List_iterator(self->controls);
    while (PC_ListIterator_moveNext(ci))
    {
        void *c = PC_ListIterator_current(ci);
        PG_Extents cm;
        PG_Control_margin(c, &cm);
        if (self->orientation == PG_BO_Horizontal)
        {
            minWidth += PG_Control_minWidth(c) + cm.left + cm.right;
            unsigned int cmh = PG_Control_minHeight(c) + cm.top + cm.bottom;
            minHeight = cmh > minHeight ? cmh : minHeight;
        }
        else
        {
            minHeight += PG_Control_minHeight(c) + cm.top + cm.bottom;
            unsigned int cmw = PG_Control_minWidth(c) + cm.left + cm.right;
            minWidth = cmw > minWidth ? cmw : minWidth;
        }
    }
    PC_ListIterator_destroy(ci);
    privateApi.control.setContentSize(self, minWidth, minHeight);
}

static void onElementMinSizeChanged(void *selfPtr, PC_EventArgs *args)
{
    (void)args;
    PG_BoxLayout *self = selfPtr;
    updateContentSize(self);
    PG_Bounds b;
    PG_Control_bounds(self, &b);
    updateElementsBounds(self, &b);
}

static void *boxElementCreator(PG_BoxLayout *self, void *control)
{
    PG_BoxElement *be = malloc(sizeof(PG_BoxElement));
    GCINIT(be);
    privateApi.container.create(be);
    privateApi.setControlObject(be, privateApi.controlObject(self));
    PG_Container_setControl(be, control);
    be->box = self;
    PC_Event_register(PG_Control_minSizeChangedEvent(control),
            self, onElementMinSizeChanged);
    return be;
}

static void boxElementDeleter(void *element)
{
    PG_BoxElement *be = element;
    void *control = PG_Container_control(be);
    PC_Event_unregister(PG_Control_minSizeChangedEvent(control),
            be->box, onElementMinSizeChanged);
    PG_Container_setControl(be, 0);
    privateApi.container.destroy(be);
    free(be);
}

static int boxElementMatcher(const void *element, void *control)
{
    return PG_Container_control(element) == control;
}

static void onContainerChanged(void *selfPtr, PC_EventArgs *args)
{
    (void)args;
    PG_BoxLayout *self = selfPtr;
    PC_ListIterator *ci = PC_List_iterator(self->controls);
    while (PC_ListIterator_moveNext(ci))
    {
        void *control = PC_ListIterator_current(ci);
        void *container = privateApi.control.container(control);
        privateApi.control.setContainer(control, 0);
        privateApi.control.setContainer(control, container);
    }
    PC_ListIterator_destroy(ci);
}

static void onResized(void *selfPtr, PC_EventArgs *args)
{
    PG_BoxLayout *self = selfPtr;
    PG_Bounds *b = args->evInfo;
    updateElementsBounds(self, b);
}

SOEXPORT PG_BoxLayout *PG_BoxLayout_create(PG_BoxOrientation orientation, int expand)
{
    PG_BoxLayout *self = malloc(sizeof(PG_BoxLayout));
    GCINIT(self);
    privateApi.control.create(self);
    self->orientation = orientation;
    self->expand = expand;
    self->elements = PC_List_create(0, boxElementDeleter, 0);
    self->controls = PC_List_create(0, 0, 0);
    PC_Event_register(PG_Control_containerChangedEvent(self),
            self, onContainerChanged);
    PC_Event_register(PG_Control_resizedEvent(self),
            self, onResized);
    return self;
}

SOEXPORT PC_ListIterator *PG_BoxLayout_controls(const PG_BoxLayout *self)
{
    return PC_List_iterator(self->controls);
}

SOEXPORT void PG_BoxLayout_removeControl(PG_BoxLayout *self, void *control)
{
    PC_List_removeMatching(self->elements, boxElementMatcher, control);
    PC_List_remove(self->controls, control);
    PG_Bounds b;
    PG_Control_bounds(self, &b);
    updateContentSize(self);
    updateElementsBounds(self, &b);
}

SOEXPORT void PG_BoxLayout_addControl(PG_BoxLayout *self, void *control)
{
    PC_List_removeMatching(self->elements, boxElementMatcher, control);
    PC_List_remove(self->controls, control);
    PC_List_append(self->controls, control);
    PC_List_append(self->elements, boxElementCreator(self, control));
    PG_Bounds b;
    PG_Control_bounds(self, &b);
    updateContentSize(self);
    updateElementsBounds(self, &b);
}

SOEXPORT void PG_BoxLayout_destroy(PG_BoxLayout *self)
{
    if (!self) return;
    PC_Event_unregister(PG_Control_resizedEvent(self),
            self, onResized);
    PC_Event_unregister(PG_Control_containerChangedEvent(self),
            self, onContainerChanged);
    PC_List_destroy(self->elements);
    PC_List_destroy(self->controls);
    privateApi.control.destroy(self);
    free(self);
}
