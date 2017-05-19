#include <stdlib.h>

#include <pocas/core/event.h>

#include <pocas/gui/command.h>

#include "internal.h"

struct PG_Command
{
    GuiClass gc;
    PC_Event *invoked;
};

SOEXPORT PG_Command *PG_Command_create(void)
{
    PG_Command *self = malloc(sizeof(PG_Command));
    GCINIT(self);
    self->invoked = PC_Event_create("invoked");
    return self;
}

SOEXPORT void PG_Command_invoke(PG_Command *self)
{
    PC_EventArgs args = PC_EventArgs_init(self->invoked, self, 0);
    PC_Event_raise(self->invoked, &args);
}


SOEXPORT PC_Event *PG_Command_invokedEvent(const PG_Command *self)
{
    return self->invoked;
}

SOEXPORT void PG_Command_destroy(PG_Command *self)
{
    if (!self) return;
    PC_Event_destroy(self->invoked);
    free(self);
}
