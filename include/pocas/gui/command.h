#ifndef POCAS_GUI_COMMAND_H
#define POCAS_GUI_COMMAND_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PC_Event);

C_CLASS_DECL(PG_Command);

DECLEXPORT PG_Command *PG_Command_create(void);
DECLEXPORT void PG_Command_invoke(PG_Command *self);
DECLEXPORT PC_Event *PG_Command_invokedEvent(const PG_Command *self);
DECLEXPORT void PG_Command_destroy(PG_Command *self);

#endif
