#ifndef POCAS_GUI_BUTTON_H
#define POCAS_GUI_BUTTON_H

#include <pocas/gui/decl.h>

typedef struct Button Button;

typedef struct Command Command;
typedef struct Event Event;

typedef enum ButtonStyle
{
    BS_Normal,
    BS_Default
} ButtonStyle;

DECLEXPORT Button *Button_create(ButtonStyle style, const char *text);
DECLEXPORT ButtonStyle Button_style(const Button *self);
DECLEXPORT const char *Button_text(const Button *self);
DECLEXPORT void Button_setText(Button *self, const char *text);
DECLEXPORT Command *Button_command(const Button *self);
DECLEXPORT void Button_setCommand(Button *self, Command *command);
DECLEXPORT void Button_click(Button *self);
DECLEXPORT Event *Button_clickedEvent(const Button *self);
DECLEXPORT void Button_destroy(Button *self);

#endif
