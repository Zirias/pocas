#ifndef POCAS_GUI_BUTTON_H
#define POCAS_GUI_BUTTON_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PC_Event);

C_CLASS_DECL(PG_Command);

C_CLASS_DECL(PG_Button);

enum PG_ButtonStyle
{
    PG_BS_Normal,
    PG_BS_Default
};
C_ENUM_DECL(PG_ButtonStyle);

#ifndef POCAS_GUI_PRIVATE

DECLEXPORT PG_Button *PG_Button_create(PG_ButtonStyle style, const char *text);
DECLEXPORT PG_ButtonStyle PG_Button_style(const PG_Button *self);
DECLEXPORT const char *PG_Button_text(const PG_Button *self);
DECLEXPORT void PG_Button_setText(PG_Button *self, const char *text);
DECLEXPORT PG_Command *PG_Button_command(const PG_Button *self);
DECLEXPORT void PG_Button_setCommand(PG_Button *self, PG_Command *command);
DECLEXPORT void PG_Button_click(PG_Button *self);
DECLEXPORT PC_Event *PG_Button_clickedEvent(const PG_Button *self);
DECLEXPORT void PG_Button_destroy(PG_Button *self);

#endif

#endif
