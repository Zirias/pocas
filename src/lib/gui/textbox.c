#include <stdlib.h>
#include <string.h>

#include <pocas/core/event.h>
#include <pocas/core/string.h>

#include <pocas/gui/backend.h>
#include <pocas/gui/control.h>
#include <pocas/gui/textbox.h>

#include "internal.h"

struct PG_TextBox
{
    GuiClass gc;
    PG_TextBoxStyle style;
    char *text;
    PC_Event *textChanged;
};

static void updateText(PG_TextBox *self, const char *text)
{
    char *oldText = self->text;
    self->text = text ? PC_String_copy(text) : 0;
    if ((text && !oldText) || (oldText && !text)
        || (oldText && text && strcmp(oldText, text)))
    {
        PC_EventArgs args = PC_EventArgs_init(
                self->textChanged, self, self->text);
        PC_Event_raise(self->textChanged, &args);
    }
    free(oldText);
}

SOEXPORT PG_TextBox *PG_TextBox_create(PG_TextBoxStyle style)
{
    PG_TextBox *self = malloc(sizeof(PG_TextBox));
    GCINIT(self);
    privateApi.control.create(self);
    self->style = style;
    self->text = 0;
    self->textChanged = PC_Event_create("textChanged");

    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.textBox.create)
        b->backendApi.textBox.create(self, updateText);

    return self;
}

SOEXPORT PG_TextBoxStyle PG_TextBox_style(const PG_TextBox *self)
{
    return self->style;
}

SOEXPORT const char *PG_TextBox_text(const PG_TextBox *self)
{
    return self->text;
}

SOEXPORT void PG_TextBox_setText(PG_TextBox *self, const char *text)
{
    updateText(self, text);
    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.textBox.setText)
        b->backendApi.textBox.setText(self, text);
}

SOEXPORT PC_Event *PG_TextBox_textChangedEvent(const PG_TextBox *self)
{
    return self->textChanged;
}

SOEXPORT void PG_TextBox_destroy(PG_TextBox *self)
{
    if (!self) return;
    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.textBox.destroy)
        b->backendApi.textBox.destroy(self);
    PC_Event_destroy(self->textChanged);
    free(self->text);
    privateApi.control.destroy(self);
    free(self);
}
