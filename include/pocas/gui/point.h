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

#define PG_Point_invalidX INT_MIN
#define PG_Point_invalidY INT_MIN
#ifdef __cplusplus
#define PG_Point_invalidPoint {PG_Point_invalidX, PG_Point_invalidY}
#else
#define PG_Point_invalidPoint {.x=PG_Point_invalidX, .y=PG_Point_invalidY}
#endif

#define PG_Point_xValid(p) ((p)->x != PG_Point_invalidX)
#define PG_Point_yValid(p) ((p)->y != PG_Point_invalidY)
#define PG_Point_valid(p) (PG_Point_xValid((p)) && PG_Point_yValid((p)))
#define PG_Point_setInvalid(p) do { \
    (p)->x = PG_Point_invalidX; \
    (p)->y = PG_Point_invalidY; \
    } while (0)
#define PG_Point_equals(p, other) \
    ((p)->x == (other)->x && (p)->y == (other)->y)

#ifdef __cplusplus
#define PG_Point_init(ix, iy) {(ix), (iy)}
#else
#define PG_Point_init(ix, iy) {.x=(ix), .y=(iy)}
#endif

#endif
