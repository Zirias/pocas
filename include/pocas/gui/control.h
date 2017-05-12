#ifndef POCAS_GUI_CONTROL_H
#define POCAS_GUI_CONTROL_H

#include <pocas/gui/decl.h>

typedef struct Bounds Bounds;
typedef struct Event Event;
typedef struct Extents Extents;

typedef enum ControlDockMode
{
    CDM_None = 0,
    CDM_Top = 1 << 0,
    CDM_Bottom = 1 << 1,
    CDM_Left = 1 << 2,
    CDM_Right = 1 << 3,
    CDM_Vertical = CDM_Top | CDM_Bottom,
    CDM_Horizontal = CDM_Left | CDM_Right,
    CDM_All = CDM_Vertical | CDM_Horizontal
} ControlDockMode;

typedef enum ControlAlignment
{
    CA_Left = 0,
    CA_Middle = 0,
    CA_Right = 1 << 0,
    CA_Center = 2 << 1,
    CA_Top = 2 << 2,
    CA_Bottom = 2 << 3
} ControlAlignment;

#define MIN_AUTO 0

DECLEXPORT ControlDockMode Control_dockMode(const void *self);
DECLEXPORT void Control_setDockMode(void *self, ControlDockMode mode);
DECLEXPORT ControlAlignment Control_alignment(const void *self);
DECLEXPORT void Control_setAlignment(void *self, ControlAlignment alignment);
DECLEXPORT void Control_bounds(const void *self, Bounds *b);
DECLEXPORT void Control_setBounds(void *self, const Bounds *b);
DECLEXPORT void Control_margin(const void *self, Extents *e);
DECLEXPORT void Control_setMargin(void *self, const Extents *e);
DECLEXPORT void Control_padding(const void *self, Extents *e);
DECLEXPORT void Control_setPadding(void *self, const Extents *e);
DECLEXPORT unsigned int Control_minWidth(const void *self);
DECLEXPORT unsigned int Control_minHeight(const void *self);
DECLEXPORT void Control_setMinSize(void *self,
        unsigned int minWidth, unsigned int minHeight);

DECLEXPORT int Control_shown(const void *self);
DECLEXPORT void Control_setShown(void *self, int shown);
#define Control_show(c) Control_setShown((c),1)
#define Control_hide(c) Control_setShown((c),0)

DECLEXPORT int Control_enabled(const void *self);
DECLEXPORT void Control_setEnabled(void *self, int enabled);
#define Control_enable(c) Control_setEnabled((c),1)
#define Control_disable(c) Control_setEnabled((c),0)

DECLEXPORT void Control_focus(void *self);

DECLEXPORT Event *Control_resizedEvent(const void *self);
DECLEXPORT Event *Control_shownChangedEvent(const void *self);
DECLEXPORT Event *Control_containerChangedEvent(const void *self);
DECLEXPORT Event *Control_minSizeChangedEvent(const void *self);

#endif
