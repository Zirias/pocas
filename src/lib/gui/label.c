#include <stdlib.h>
#include <string.h>

#include <pocas/core/string.h>

#include "internal.h"
#include <pocas/gui/label.h>

struct PG_Label
{
    GuiClass gc;
    char *text;
};

SOEXPORT PG_Label *PG_Label_create(const char *text)
{
    PG_Label *self = malloc(sizeof(PG_Label));
    GCINIT(self);
    privateApi.control.create(self);
    self->text = text ? PC_String_copy(text) : 0;
    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.label.create) b->backendApi.label.create(self);
    return self;
}

SOEXPORT const char *PG_Label_text(const PG_Label *self)
{
    return self->text;
}

SOEXPORT void PG_Label_setText(PG_Label *self, const char *text)
{
    free(self->text);
    self->text = text ? PC_String_copy(text) : 0;
    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.label.setText) b->backendApi.label.setText(self, text);
}

SOEXPORT void PG_Label_destroy(PG_Label *self)
{
    if (!self) return;
    const PG_Backend *b = PG_Backend_current();
    if (b->backendApi.label.destroy) b->backendApi.label.destroy(self);
    privateApi.control.destroy(self);
    free(self->text);
    free(self);
}
