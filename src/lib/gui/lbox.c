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

struct LBox
{
    GuiClass gc;
    BoxOrientation orientation;
    List *elements;
    List *controls;
};

typedef struct LBoxElement
{
    GuiClass gc;
    LBox *box;
} LBoxElement;

static void updateElementsBounds(LBox *self, Bounds *b)
{
    if (!List_length(self->elements)) return;
    Bounds eb;
    eb.x = b->x;
    eb.y = b->y;
    Extents em;
    ListIterator *ei = List_iterator(self->elements);
    while (ListIterator_moveNext(ei))
    {
        GuiClass *lbe = ListIterator_current(ei);
        void *control = Container_control(lbe);
        Control_margin(control, &em);
        eb.width = Control_minWidth(control) + em.left + em.right;
        if (eb.x + eb.width > b->x + b->width)
        {
            if (eb.x >= b->x + b->width) eb.width = 0;
            else eb.width = b->x + b->width - eb.x;
        }
        eb.height = Control_minHeight(control) + em.top + em.bottom;
        if (eb.y + eb.height > b->y + b->height)
        {
            if (eb.y >= b->y + b->height) eb.height = 0;
            else eb.height = b->y + b->height - eb.y;
        }
        privateApi.container.setBounds(lbe, &eb);
        if (self->orientation == BO_Horizontal)
        {
            eb.x += eb.width;
        }
        else
        {
            eb.y += eb.height;
        }
    }
    ListIterator_destroy(ei);
}

static void updateContentSize(LBox *self)
{
    unsigned int minWidth = 0;
    unsigned int minHeight = 0;
    ListIterator *ci = List_iterator(self->controls);
    while (ListIterator_moveNext(ci))
    {
        void *c = ListIterator_current(ci);
        Extents cm;
        Control_margin(c, &cm);
        if (self->orientation == BO_Horizontal)
        {
            minWidth += Control_minWidth(c) + cm.left + cm.right;
            unsigned int cmh = Control_minHeight(c) + cm.top + cm.bottom;
            minHeight = cmh > minHeight ? cmh : minHeight;
        }
        else
        {
            minHeight += Control_minHeight(c) + cm.top + cm.bottom;
            unsigned int cmw = Control_minWidth(c) + cm.left + cm.right;
            minWidth = cmw > minWidth ? cmw : minWidth;
        }
    }
    ListIterator_destroy(ci);
    privateApi.control.setContentSize(self, minWidth, minHeight);
}

static void onElementMinSizeChanged(void *selfPtr, EventArgs *args)
{
    (void)args;
    LBox *self = selfPtr;
    updateContentSize(self);
    Bounds b;
    Control_bounds(self, &b);
    updateElementsBounds(self, &b);
}

static void *lboxElementCreator(LBox *self, void *control)
{
    LBoxElement *lbe = malloc(sizeof(LBoxElement));
    GCINIT(lbe);
    privateApi.container.create(lbe);
    privateApi.setControlObject(lbe, privateApi.controlObject(self));
    Container_setControl(lbe, control);
    lbe->box = self;
    Event_register(Control_minSizeChangedEvent(control),
            self, onElementMinSizeChanged);
    return lbe;
}

static void lboxElementDeleter(void *element)
{
    LBoxElement *lbe = element;
    void *control = Container_control(lbe);
    Event_unregister(Control_minSizeChangedEvent(control),
            lbe->box, onElementMinSizeChanged);
    Container_setControl(lbe, 0);
    privateApi.container.destroy(lbe);
    free(lbe);
}

static int lboxElementMatcher(const void *element, void *control)
{
    return Container_control(element) == control;
}

static void onContainerChanged(void *selfPtr, EventArgs *args)
{
    (void)args;
    LBox *self = selfPtr;
    ListIterator *ci = List_iterator(self->controls);
    while (ListIterator_moveNext(ci))
    {
        void *control = ListIterator_current(ci);
        void *container = privateApi.control.container(control);
        privateApi.control.setContainer(control, 0);
        privateApi.control.setContainer(control, container);
    }
    ListIterator_destroy(ci);
}

static void onResized(void *selfPtr, EventArgs *args)
{
    LBox *self = selfPtr;
    Bounds *b = args->evInfo;
    updateElementsBounds(self, b);
}

SOEXPORT LBox *LBox_create(BoxOrientation orientation)
{
    LBox *self = malloc(sizeof(LBox));
    GCINIT(self);
    privateApi.control.create(self);
    self->orientation = orientation;
    self->elements = List_create(0, lboxElementDeleter, 0);
    self->controls = List_create(0, 0, 0);
    Event_register(Control_containerChangedEvent(self),
            self, onContainerChanged);
    Event_register(Control_resizedEvent(self),
            self, onResized);
    return self;
}

SOEXPORT ListIterator *LBox_controls(const LBox *self)
{
    return List_iterator(self->controls);
}

SOEXPORT void LBox_removeControl(LBox *self, void *control)
{
    List_removeMatching(self->elements, lboxElementMatcher, control);
    List_remove(self->controls, control);
    Bounds b;
    Control_bounds(self, &b);
    updateElementsBounds(self, &b);
}

SOEXPORT void LBox_addControl(LBox *self, void *control)
{
    List_removeMatching(self->elements, lboxElementMatcher, control);
    List_remove(self->controls, control);
    List_append(self->controls, control);
    List_append(self->elements, lboxElementCreator(self, control));
    Bounds b;
    Control_bounds(self, &b);
    updateElementsBounds(self, &b);
}

SOEXPORT void LBox_destroy(LBox *self)
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
