#ifndef POCAS_GUI_SIZE_H
#define POCAS_GUI_SIZE_H

#include <limits.h>

#include <pocas/gui/decl.h>

C_CLASS_DECL(PG_Size);
struct PG_Size
{
    int width;
    int height;
};

#define PG_Size_widthValid(s) ((s)->width > INT_MIN)
#define PG_Size_heightValid(s) ((s)->height > INT_MIN)
#define PG_Size_valid(s) (PG_Size_widthValid((s)) && PG_Size_heightValid((s)))
#define PG_Size_equals(s, other) ((s)->width == (other)->width && (s)->height == (other)->height)

#define PG_Size_init(w, h) {.width=(w), .height=(h)}
#define PG_Size_invalidSize {INT_MIN, INT_MIN}

#endif
