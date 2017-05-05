#ifndef POCAS_GUI_BOUNDS_H
#define POCAS_GUI_BOUNDS_H

#include <pocas/gui/decl.h>

typedef struct Bounds
{
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
} Bounds;

DECLEXPORT int Bounds_equals(const Bounds *self, const Bounds *other);
DECLEXPORT void Bounds_fitInto(Bounds *self, const Bounds *into);

#endif
