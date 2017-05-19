#ifndef POCAS_GUI_LABEL_H
#define POCAS_GUI_LABEL_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PG_Label);

enum PG_LabelStyle
{
    PG_LS_Normal = 0,
    PG_LS_HCenter = 1 << 0,
    PG_LS_VCenter = 1 << 1,
    PG_LS_Center = PG_LS_HCenter|PG_LS_VCenter,
    PG_LS_AlignBottom = 1 << 2,
    PG_LS_AlignRight = 1 << 3
};
C_ENUM_DECL(PG_LabelStyle);

#ifndef POCAS_GUI_PRIVATE

DECLEXPORT PG_Label *PG_Label_create(PG_LabelStyle style, const char *text);
DECLEXPORT PG_LabelStyle PG_Label_style(const PG_Label *self);
DECLEXPORT const char *PG_Label_text(const PG_Label *self);
DECLEXPORT void PG_Label_setText(PG_Label *self, const char *text);
DECLEXPORT void PG_Label_destroy(PG_Label *self);

#endif

#endif
