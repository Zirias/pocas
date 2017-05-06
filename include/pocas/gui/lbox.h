#ifndef POCAS_GUI_LBOX_H
#define POCAS_GUI_LBOX_H

#include <pocas/gui/decl.h>

typedef struct LBox LBox;

typedef struct ListIterator ListIterator;

typedef enum BoxOrientation
{
    BO_Horizontal = 0,
    BO_Vertical
} BoxOrientation;

DECLEXPORT LBox *LBox_create(BoxOrientation orientation);

DECLEXPORT ListIterator *LBox_controls(const LBox *self);
DECLEXPORT void LBox_addControl(LBox *self, void *control);
DECLEXPORT void LBox_removeControl(LBox *self, void *control);

DECLEXPORT void LBox_destroy(LBox *self);

#endif
