#include <stdlib.h>
#include <string.h>

#include <pocas/core/event.h>
#include <pocas/core/list.h>

#include "internal.h"
#include <pocas/gui/bounds.h>
#include <pocas/gui/container.h>
#include <pocas/gui/control.h>
#include <pocas/gui/extents.h>
#include <pocas/gui/lbox.h>

struct PG_LBox
{
    GuiClass gc;
    PG_BoxOrientation orientation;
    PC_List *elements;
    PC_List *controls;
};

typedef struct PG_LBoxElement
{
    GuiClass gc;
    PG_LBox *box;
} PG_LBoxElement;

static void updateElementsBounds(PG_LBox *self, PG_Bounds *b)
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
        eb.width = PG_Control_minWidth(control) + em.left + em.right;
        if (eb.x + eb.width > b->x + b->width)
        {
            if (eb.x >= b->x + b->width) eb.width = 0;
            else eb.width = b->x + b->width - eb.x;
        }
        eb.height = PG_Control_minHeight(control) + em.top + em.bottom;
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

static void updateContentSize(PG_LBox *self)
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
    PG_LBox *self = selfPtr;
    updateContentSize(self);
    PG_Bounds b;
    PG_Control_bounds(self, &b);
    updateElementsBounds(self, &b);
}

static void *lboxElementCreator(PG_LBox *self, void *control)
{
    PG_LBoxElement *lbe = malloc(sizeof(PG_LBoxElement));
    GCINIT(lbe);
    privateApi.container.create(lbe);
    privateApi.setControlObject(lbe, privateApi.controlObject(self));
    PG_Container_setControl(lbe, control);
    lbe->box = self;
    PC_Event_register(PG_Control_minSizeChangedEvent(control),
            self, onElementMinSizeChanged);
    return lbe;
}

static void lboxElementDeleter(void *element)
{
    PG_LBoxElement *lbe = element;
    void *control = PG_Container_control(lbe);
    PC_Event_unregister(PG_Control_minSizeChangedEvent(control),
            lbe->box, onElementMinSizeChanged);
    PG_Container_setControl(lbe, 0);
    privateApi.container.destroy(lbe);
    free(lbe);
}

static int lboxElementMatcher(const void *element, void *control)
{
    return PG_Container_control(element) == control;
}

static void onContainerChanged(void *selfPtr, PC_EventArgs *args)
{
    (void)args;
    PG_LBox *self = selfPtr;
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
    PG_LBox *self = selfPtr;
    PG_Bounds *b = args->evInfo;
    updateElementsBounds(self, b);
}

SOEXPORT PG_LBox *PG_LBox_create(PG_BoxOrientation orientation)
{
    PG_LBox *self = malloc(sizeof(PG_LBox));
    GCINIT(self);
    privateApi.control.create(self);
    self->orientation = orientation;
    self->elements = PC_List_create(0, lboxElementDeleter, 0);
    self->controls = PC_List_create(0, 0, 0);
    PC_Event_register(PG_Control_containerChangedEvent(self),
            self, onContainerChanged);
    PC_Event_register(PG_Control_resizedEvent(self),
            self, onResized);
    return self;
}

SOEXPORT PC_ListIterator *PG_LBox_controls(const PG_LBox *self)
{
    return PC_List_iterator(self->controls);
}

SOEXPORT void PG_LBox_removeControl(PG_LBox *self, void *control)
{
    PC_List_removeMatching(self->elements, lboxElementMatcher, control);
    PC_List_remove(self->controls, control);
    PG_Bounds b;
    PG_Control_bounds(self, &b);
    updateContentSize(self);
    updateElementsBounds(self, &b);
}

SOEXPORT void PG_LBox_addControl(PG_LBox *self, void *control)
{
    PC_List_removeMatching(self->elements, lboxElementMatcher, control);
    PC_List_remove(self->controls, control);
    PC_List_append(self->controls, control);
    PC_List_append(self->elements, lboxElementCreator(self, control));
    PG_Bounds b;
    PG_Control_bounds(self, &b);
    updateContentSize(self);
    updateElementsBounds(self, &b);
}

SOEXPORT void PG_LBox_destroy(PG_LBox *self)
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
