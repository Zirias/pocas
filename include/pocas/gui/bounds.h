#ifndef POCAS_GUI_BOUNDS_H
#define POCAS_GUI_BOUNDS_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PG_Bounds);
struct PG_Bounds
{
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
};

DECLEXPORT int PG_Bounds_equals(const PG_Bounds *self, const PG_Bounds *other);
DECLEXPORT void PG_Bounds_fitInto(PG_Bounds *self, const PG_Bounds *into);

#endif
