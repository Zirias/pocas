#include <stdlib.h>
#include <string.h>

#include <pocas/core/string.h>

#include "internal.h"
#include <pocas/gui/label.h>

struct Label
{
    GuiClass gc;
    char *text;
};

SOEXPORT Label *Label_create(const char *text)
{
    Label *self = malloc(sizeof(Label));
    GCINIT(self);
    privateApi.control.create(self);
    self->text = text ? String_copy(text) : 0;
    const Backend *b = Backend_current();
    if (b->backendApi.label.create) b->backendApi.label.create(self);
    return self;
}

SOEXPORT const char *Label_text(const Label *self)
{
    return self->text;
}

SOEXPORT void Label_setText(Label *self, const char *text)
{
    free(self->text);
    self->text = text ? String_copy(text) : 0;
    const Backend *b = Backend_current();
    if (b->backendApi.label.setText) b->backendApi.label.setText(self, text);
}

SOEXPORT void Label_destroy(Label *self)
{
    if (!self) return;
    const Backend *b = Backend_current();
    if (b->backendApi.label.destroy) b->backendApi.label.destroy(self);
    privateApi.control.destroy(self);
    free(self->text);
    free(self);
}
