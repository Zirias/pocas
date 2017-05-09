#ifndef POCAS_GUI_TEXTBOX_H
#define POCAS_GUI_TEXTBOX_H

#include <pocas/gui/decl.h>

typedef struct Event Event;

typedef struct TextBox TextBox;

typedef enum TextBoxStyle
{
    TBS_Normal = 0,
    TBS_Multiline = 1 << 0,
} TextBoxStyle;

DECLEXPORT TextBox *TextBox_create(TextBoxStyle style);
DECLEXPORT TextBoxStyle TextBox_style(const TextBox *self);
DECLEXPORT const char *TextBox_text(const TextBox *self);
DECLEXPORT void TextBox_setText(TextBox *self, const char *text);
DECLEXPORT Event *TextBox_textChangedEvent(const TextBox *self);
DECLEXPORT void TextBox_destroy(TextBox *self);

#endif
