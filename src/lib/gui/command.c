#include <stdlib.h>

#include <pocas/core/event.h>

#include "internal.h"
#include <pocas/gui/command.h>

struct Command
{
    GuiClass gc;
    Event *invoked;
};

SOEXPORT Command *Command_create(void)
{
    Command *self = malloc(sizeof(Command));
    GCINIT(self);
    self->invoked = Event_create("invoked");
    return self;
}

SOEXPORT void Command_invoke(Command *self)
{
    EventArgs *args = EventArgs_create(self->invoked, self, 0);
    Event_raise(self->invoked, args);
    EventArgs_destroy(args);
}


SOEXPORT Event *Command_invokedEvent(const Command *self)
{
    return self->invoked;
}

SOEXPORT void Command_destroy(Command *self)
{
    if (!self) return;
    Event_destroy(self->invoked);
    free(self);
}
