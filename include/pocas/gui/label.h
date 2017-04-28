#ifndef POCAS_GUI_LABEL_H
#define POCAS_GUI_LABEL_H

#include <pocas/gui/decl.h>

typedef struct Label Label;

Label *Label_create(const char *text);
const char *Label_text(const Label *self);
void Label_setText(Label *self, const char *text);
void Label_destroy(Label *self);

#endif
