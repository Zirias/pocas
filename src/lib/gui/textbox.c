#include <stdlib.h>
#include <string.h>

#include <pocas/core/event.h>
#include <pocas/core/string.h>

#include "internal.h"
#include <pocas/gui/control.h>
#include <pocas/gui/textbox.h>

struct TextBox
{
    GuiClass gc;
    TextBoxStyle style;
    char *text;
    Event *textChanged;
};

static void updateText(TextBox *self, const char *text)
{
    char *oldText = self->text;
    self->text = text ? String_copy(text) : 0;
    if ((text && !oldText) || (oldText && !text)
        || (oldText && text && strcmp(oldText, text)))
    {
        EventArgs args = EventArgs_init(
                self->textChanged, self, self->text);
        Event_raise(self->textChanged, &args);
    }
    free(oldText);
}

SOEXPORT TextBox *TextBox_create(TextBoxStyle style)
{
    TextBox *self = malloc(sizeof(TextBox));
    GCINIT(self);
    privateApi.control.create(self);
    self->style = style;
    self->text = 0;
    self->textChanged = Event_create("textChanged");

    const Backend *b = Backend_current();
    if (b->backendApi.textBox.create)
        b->backendApi.textBox.create(self, updateText);

    return self;
}

SOEXPORT TextBoxStyle TextBox_style(const TextBox *self)
{
    return self->style;
}

SOEXPORT const char *TextBox_text(const TextBox *self)
{
    return self->text;
}

SOEXPORT void TextBox_setText(TextBox *self, const char *text)
{
    updateText(self, text);
    const Backend *b = Backend_current();
    if (b->backendApi.textBox.setText)
        b->backendApi.textBox.setText(self, text);
}

SOEXPORT Event *TextBox_textChangedEvent(const TextBox *self)
{
    return self->textChanged;
}

SOEXPORT void TextBox_destroy(TextBox *self)
{
    if (!self) return;
    const Backend *b = Backend_current();
    if (b->backendApi.textBox.destroy)
        b->backendApi.textBox.destroy(self);
    Event_destroy(self->textChanged);
    free(self->text);
    privateApi.control.destroy(self);
    free(self);
}
