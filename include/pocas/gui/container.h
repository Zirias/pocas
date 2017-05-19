#ifndef POCAS_GUI_CONTAINER_H
#define POCAS_GUI_CONTAINER_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PC_Event);

C_CLASS_DECL(PG_Bounds);

#ifndef POCAS_GUI_PRIVATE

DECLEXPORT void *PG_Container_control(const void *self);
DECLEXPORT void PG_Container_setControl(void *self, void *control);
DECLEXPORT void PG_Container_bounds(const void *self, PG_Bounds *b);
DECLEXPORT PC_Event *PG_Container_resizedEvent(const void *self);

#endif

#endif
