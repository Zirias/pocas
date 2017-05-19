#include <stdlib.h>

#include <pocas/core/event.h>
#include <pocas/core/string.h>

#include <pocas/gui/backend.h>
#include <pocas/gui/control.h>
#include <pocas/gui/command.h>
#include <pocas/gui/extents.h>

#include <pocas/gui/button.h>

#include "internal.h"

struct PG_Button
{
    GuiClass gc;
    PG_ButtonStyle style;
    char *text;
    PG_Command *command;
    PC_Event *clicked;
};

SOEXPORT PG_Button *PG_Button_create(PG_ButtonStyle style, const char *text)
{
    PG_Button *self = malloc(sizeof(PG_Button));
    GCINIT(self);
    privateApi.control.create(self);
    self->style = style;
    self->text = text ? PC_String_copy(text) : 0;
    self->command = 0;
    self->clicked = PC_Event_create("clicked");

    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.button.create)
        b->backendApi.button.create(self);
    return self;
}

SOEXPORT PG_ButtonStyle PG_Button_style(const PG_Button *self)
{
    return self->style;
}

SOEXPORT const char *PG_Button_text(const PG_Button *self)
{
    return self->text;
}

SOEXPORT void PG_Button_setText(PG_Button *self, const char *text)
{
    free(self->text);
    self->text = text ? PC_String_copy(text) : 0;

    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.button.setText)
        b->backendApi.button.setText(self, text);
}

SOEXPORT PG_Command *PG_Button_command(const PG_Button *self)
{
    return self->command;
}

SOEXPORT void PG_Button_setCommand(PG_Button *self, PG_Command *command)
{
    self->command = command;
}

SOEXPORT void PG_Button_click(PG_Button *self)
{
    PC_EventArgs args = PC_EventArgs_init(self->clicked, self, 0);
    PC_Event_raise(self->clicked, &args);
    if (!args.handled && self->command)
    {
        PG_Command_invoke(self->command);
    }
}

SOEXPORT PC_Event *PG_Button_clickedEvent(const PG_Button *self)
{
    return self->clicked;
}

SOEXPORT void PG_Button_destroy(PG_Button *self)
{
    if (!self) return;
    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.button.destroy)
        b->backendApi.button.destroy(self);
    PC_Event_destroy(self->clicked);
    privateApi.control.destroy(self);
    free(self->text);
    free(self);
}
