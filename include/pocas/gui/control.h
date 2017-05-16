#ifndef POCAS_GUI_CONTROL_H
#define POCAS_GUI_CONTROL_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PC_Event);

C_CLASS_DECL(PG_Bounds);
C_CLASS_DECL(PG_Extents);

enum PG_ControlDockMode
{
    PG_CDM_None = 0,
    PG_CDM_Top = 1 << 0,
    PG_CDM_Bottom = 1 << 1,
    PG_CDM_Left = 1 << 2,
    PG_CDM_Right = 1 << 3,
    PG_CDM_Vertical = PG_CDM_Top | PG_CDM_Bottom,
    PG_CDM_Horizontal = PG_CDM_Left | PG_CDM_Right,
    PG_CDM_All = PG_CDM_Vertical | PG_CDM_Horizontal
};
C_ENUM_DECL(PG_ControlDockMode);

enum PG_ControlAlignment
{
    PG_CA_Left = 0,
    PG_CA_Middle = 0,
    PG_CA_Right = 1 << 0,
    PG_CA_Center = 1 << 1,
    PG_CA_Top = 1 << 2,
    PG_CA_Bottom = 1 << 3
};
C_ENUM_DECL(PG_ControlAlignment);

#define PG_MIN_AUTO 0

DECLEXPORT PG_ControlDockMode PG_Control_dockMode(const void *self);
DECLEXPORT void PG_Control_setDockMode(void *self, PG_ControlDockMode mode);
DECLEXPORT PG_ControlAlignment PG_Control_alignment(const void *self);
DECLEXPORT void PG_Control_setAlignment(void *self, PG_ControlAlignment alignment);
DECLEXPORT void PG_Control_bounds(const void *self, PG_Bounds *b);
DECLEXPORT void PG_Control_setBounds(void *self, const PG_Bounds *b);
DECLEXPORT void PG_Control_margin(const void *self, PG_Extents *e);
DECLEXPORT void PG_Control_setMargin(void *self, const PG_Extents *e);
DECLEXPORT void PG_Control_padding(const void *self, PG_Extents *e);
DECLEXPORT void PG_Control_setPadding(void *self, const PG_Extents *e);
DECLEXPORT unsigned int PG_Control_minWidth(const void *self);
DECLEXPORT unsigned int PG_Control_minHeight(const void *self);
DECLEXPORT void PG_Control_setMinSize(void *self,
        unsigned int minWidth, unsigned int minHeight);

DECLEXPORT int PG_Control_shown(const void *self);
DECLEXPORT void PG_Control_setShown(void *self, int shown);
#define PG_Control_show(c) PG_Control_setShown((c),1)
#define PG_Control_hide(c) PG_Control_setShown((c),0)

DECLEXPORT int PG_Control_enabled(const void *self);
DECLEXPORT void PG_Control_setEnabled(void *self, int enabled);
#define PG_Control_enable(c) PG_Control_setEnabled((c),1)
#define PG_Control_disable(c) PG_Control_setEnabled((c),0)

DECLEXPORT void PG_Control_focus(void *self);

DECLEXPORT PC_Event *PG_Control_resizedEvent(const void *self);
DECLEXPORT PC_Event *PG_Control_shownChangedEvent(const void *self);
DECLEXPORT PC_Event *PG_Control_containerChangedEvent(const void *self);
DECLEXPORT PC_Event *PG_Control_minSizeChangedEvent(const void *self);

#endif
