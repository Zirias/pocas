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
    ButtonStyle style;
    char *text;
    Command *command;
    Event *clicked;
};

SOEXPORT Button *Button_create(ButtonStyle style, const char *text)
{
    Button *self = malloc(sizeof(Button));
    GCINIT(self);
    privateApi.control.create(self);
    self->style = style;
    self->text = text ? String_copy(text) : 0;
    self->command = 0;
    self->clicked = Event_create("clicked");

    const Backend *b = Backend_current();
    if (b->backendApi.button.create)
        b->backendApi.button.create(self);
    return self;
}

SOEXPORT ButtonStyle Button_style(const Button *self)
{
    return self->style;
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

SOEXPORT void Button_setCommand(Button *self, Command *command)
{
    self->command = command;
}

SOEXPORT void Button_click(Button *self)
{
    EventArgs args = EventArgs_init(self->clicked, self, 0);
    Event_raise(self->clicked, &args);
    if (!args.handled && self->command)
    {
        Command_invoke(self->command);
    }
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
    Event_destroy(self->clicked);
    privateApi.control.destroy(self);
    free(self->text);
    free(self);
}
