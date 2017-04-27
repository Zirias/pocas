#include <stdlib.h>

#include <pocas/core/event.h>

#include "backend_internal.h"

#include <pocas/gui/command.h>

struct Command
{
    B_Command *b;
    Event *invoked;
};

SOEXPORT Command *Command_create(void)
{
    Command *self = malloc(sizeof(Command));
    self->invoked = Event_create("invoked");
    const Backend *b = Backend_current();
    self->b = b->Command_create ? b->Command_create(self) : 0;
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
    const Backend *b = Backend_current();
    if (b->Command_destroy) b->Command_destroy(self->b);
    Event_destroy(self->invoked);
    free(self);
}
