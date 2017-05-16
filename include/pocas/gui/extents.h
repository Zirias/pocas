#ifndef POCAS_GUI_EXTENTS_H
#define POCAS_GUI_EXTENTS_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PG_Extents);
struct PG_Extents {
    unsigned int left;
    unsigned int top;
    unsigned int right;
    unsigned int bottom;
};

#endif
