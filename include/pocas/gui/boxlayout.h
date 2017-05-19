#ifndef POCAS_GUI_BOXLAYOUT_H
#define POCAS_GUI_BOXLAYOUT_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PC_ListIterator);

C_CLASS_DECL(PG_BoxLayout);

enum PG_BoxOrientation
{
    PG_BO_Horizontal = 0,
    PG_BO_Vertical
};
C_ENUM_DECL(PG_BoxOrientation);

#ifndef POCAS_GUI_PRIVATE

DECLEXPORT PG_BoxLayout *PG_BoxLayout_create(PG_BoxOrientation orientation, int expand);

DECLEXPORT PC_ListIterator *PG_BoxLayout_controls(const PG_BoxLayout *self);
DECLEXPORT void PG_BoxLayout_addControl(PG_BoxLayout *self, void *control);
DECLEXPORT void PG_BoxLayout_removeControl(PG_BoxLayout *self, void *control);

DECLEXPORT void PG_BoxLayout_destroy(PG_BoxLayout *self);

#endif

#endif
