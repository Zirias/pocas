#ifndef POCAS_GUI_TEXTBOX_H
#define POCAS_GUI_TEXTBOX_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PC_Event);

C_CLASS_DECL(PG_TextBox);

enum PG_TextBoxStyle
{
    PG_TBS_Normal = 0,
    PG_TBS_Multiline = 1 << 0,
};
C_ENUM_DECL(PG_TextBoxStyle);

DECLEXPORT PG_TextBox *PG_TextBox_create(PG_TextBoxStyle style);
DECLEXPORT PG_TextBoxStyle PG_TextBox_style(const PG_TextBox *self);
DECLEXPORT const char *PG_TextBox_text(const PG_TextBox *self);
DECLEXPORT void PG_TextBox_setText(PG_TextBox *self, const char *text);
DECLEXPORT PC_Event *PG_TextBox_textChangedEvent(const PG_TextBox *self);
DECLEXPORT void PG_TextBox_destroy(PG_TextBox *self);

#endif
