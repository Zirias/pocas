#ifndef POCAS_GUI_CONTROL_H
#define POCAS_GUI_CONTROL_H

#include <pocas/gui/decl.h>

typedef struct Bounds Bounds;
typedef struct Event Event;

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

DECLEXPORT ControlDockMode Control_dockMode(const void *self);
DECLEXPORT void Control_setDockMode(void *self, ControlDockMode mode);
DECLEXPORT void Control_bounds(const void *self, Bounds *b);
DECLEXPORT void Control_setBounds(void *self, const Bounds *b);
DECLEXPORT int Control_shown(const void *self);
DECLEXPORT void Control_setShown(void *self, int shown);

#define Control_show(c) Control_setShown((c),1)
#define Control_hide(c) Control_setShown((c),0)

DECLEXPORT Event *Control_resizedEvent(const void *self);
DECLEXPORT Event *Control_shownChangedEvent(const void *self);
DECLEXPORT Event *Control_containerChangedEvent(const void *self);

#endif
