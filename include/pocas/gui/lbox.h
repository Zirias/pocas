#ifndef POCAS_GUI_LBOX_H
#define POCAS_GUI_LBOX_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PC_ListIterator);

C_CLASS_DECL(PG_LBox);

enum PG_BoxOrientation
{
    PG_BO_Horizontal = 0,
    PG_BO_Vertical
};
C_ENUM_DECL(PG_BoxOrientation);

DECLEXPORT PG_LBox *PG_LBox_create(PG_BoxOrientation orientation);

DECLEXPORT PC_ListIterator *PG_LBox_controls(const PG_LBox *self);
DECLEXPORT void PG_LBox_addControl(PG_LBox *self, void *control);
DECLEXPORT void PG_LBox_removeControl(PG_LBox *self, void *control);

DECLEXPORT void PG_LBox_destroy(PG_LBox *self);

#endif
