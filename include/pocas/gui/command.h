#ifndef POCAS_GUI_COMMAND_H
#define POCAS_GUI_COMMAND_H

#include <pocas/gui/decl.h>

typedef struct Event Event;

typedef struct Command Command;

DECLEXPORT Command *Command_create(void);
DECLEXPORT void Command_invoke(Command *self);
DECLEXPORT Event *Command_invokedEvent(const Command *self);
DECLEXPORT void Command_destroy(Command *self);

#endif
