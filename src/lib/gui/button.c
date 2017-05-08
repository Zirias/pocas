#include <stdlib.h>

#include <pocas/core/event.h>
#include <pocas/core/string.h>

#include "internal.h"
#include <pocas/gui/control.h>
#include <pocas/gui/command.h>
#include <pocas/gui/extents.h>

#include <pocas/gui/button.h>

struct Button
{
    GuiClass gc;
    char *text;
    Command *command;
    int commandOwned;
    Event *clicked;
};

static void onButtonClicked(void *selfPtr, EventArgs *args)
{
    (void)args;
    Button *self = selfPtr;
    EventArgs *clickedArgs = EventArgs_create(self->clicked, self, 0);
    Event_raise(self->clicked, clickedArgs);
    EventArgs_destroy(clickedArgs);
}

SOEXPORT Button *Button_create(const char *text, Command *command)
{
    Button *self = malloc(sizeof(Button));
    GCINIT(self);
    privateApi.control.create(self);
    self->text = text ? String_copy(text) : 0;
    self->command = command ? command : Command_create();
    self->commandOwned = !command;
    self->clicked = Event_create("clicked");
    Event_register(Command_invokedEvent(self->command),
            self, onButtonClicked);
    Extents padding = {4,4,4,4};
    Control_setPadding(self, &padding);
    Control_setMinSize(self, 50, 0);

    const Backend *b = Backend_current();
    if (b->backendApi.button.create)
        b->backendApi.button.create(self);
    return self;
}

SOEXPORT const char *Button_text(const Button *self)
{
    return self->text;
}

SOEXPORT void Button_setText(Button *self, const char *text)
{
    free(self->text);
    self->text = text ? String_copy(text) : 0;

    const Backend *b = Backend_current();
    if (b->backendApi.button.setText)
        b->backendApi.button.setText(self, text);
}

SOEXPORT Command *Button_command(const Button *self)
{
    return self->command;
}

SOEXPORT Event *Button_clickedEvent(const Button *self)
{
    return self->clicked;
}

SOEXPORT void Button_destroy(Button *self)
{
    if (!self) return;
    const Backend *b = Backend_current();
    if (b->backendApi.button.destroy)
        b->backendApi.button.destroy(self);
    Event_unregister(Command_invokedEvent(self->command),
            self, onButtonClicked);
    if (self->commandOwned) Command_destroy(self->command);
    Event_destroy(self->clicked);
    free(self->text);
    free(self);
}
