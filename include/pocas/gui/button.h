#ifndef POCAS_GUI_BUTTON_H
#define POCAS_GUI_BUTTON_H

#include <pocas/gui/decl.h>

typedef struct Button Button;

typedef struct Command Command;
typedef struct Event Event;

DECLEXPORT Button *Button_create(const char *text, Command *command);
DECLEXPORT const char *Button_text(const Button *self);
DECLEXPORT void Button_setText(Button *self, const char *text);
DECLEXPORT Command *Button_command(const Button *self);
DECLEXPORT Event *Button_clickedEvent(const Button *self);
DECLEXPORT void Button_destroy(Button *self);

#endif
