#ifndef POCAS_GUI_POINT_H
#define POCAS_GUI_POINT_H

#include <limits.h>

#include <pocas/gui/decl.h>

C_CLASS_DECL(PG_Point);
struct PG_Point
{
    int x;
    int y;
};

#define PG_Point_xValid(p) ((p)->x > INT_MIN)
#define PG_Point_yValid(p) ((p)->y > INT_MIN)
#define PG_Point_valid(p) (PG_Point_xValid((p)) && PG_Point_yValid((p)))
#define PG_Point_equals(p, other) ((p)->x == (other)->x && (p)->y == (other)->y)

#define PG_Point_init(x, y) {.x=(x), .y=(y)}
#define PG_Point_invalidPoint {INT_MIN, INT_MIN}

#endif
