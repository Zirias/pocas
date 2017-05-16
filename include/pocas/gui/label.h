#ifndef POCAS_GUI_LABEL_H
#define POCAS_GUI_LABEL_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PG_Label);

PG_Label *PG_Label_create(const char *text);
const char *PG_Label_text(const PG_Label *self);
void PG_Label_setText(PG_Label *self, const char *text);
void PG_Label_destroy(PG_Label *self);

#endif
