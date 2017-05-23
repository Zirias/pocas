#ifndef POCAS_GUI_BOUNDS_H
#define POCAS_GUI_BOUNDS_H

#include <pocas/gui/point.h>
#include <pocas/gui/size.h>
#include <pocas/gui/extents.h>

C_CLASS_DECL(PG_Bounds);
#ifdef __cplusplus
struct PG_Bounds
{
    PG_Point topLeft;
    PG_Size size;
};
#else
struct PG_Bounds
{
    union
    {
        struct
        {
            PG_Point topLeft;
            PG_Size size;
        };
        struct
        {
            struct
            {
                int x;
                int y;
            };
            struct
            {
                int width;
                int height;
            };
        };
    };
};
#endif

#define PG_Bounds_invalidBounds \
    {.topLeft=PG_Point_invalidPoint, .size=PG_Size_invalidSize}

#define PG_Bounds_valid(b) (PG_Point_valid(&((b)->topLeft)) \
    && PG_Size_valid(&((b)->size)))
#define PG_Bounds_setInvalid(b) do { \
    PG_Point_setInvalid(&((b)->topLeft)); \
    PG_Size_setInvalid(&((b)->size)); \
    } while (0)
#define PG_Bounds_equals(b, other) \
    (PG_Point_equals(&((b)->topLeft), &((other)->topLeft)) \
    && PG_Size_equals(&((b)->size), &((other)->size)))
#define PG_Bounds_toExtents(b, e) do { \
    (e)->topLeft.x = (b)->topLeft.x; \
    (e)->topLeft.y = (b)->topLeft.y; \
    (e)->bottomRight.x = (b)->topLeft.x + (b)->size.width; \
    (e)->bottomRight.y = (b)->topLeft.y + (b)->size.height; \
    } while (0)

#define PG_Bounds_init(ix, iy, iwidth, iheight) { \
    .topLeft=PG_Point_init((ix),(iy)), \
    .size=PG_Size_init((iwidth),(iheight)) }

#ifndef POCAS_GUI_PRIVATE

DECLEXPORT void PG_Bounds_fitInto(PG_Bounds *self, const PG_Bounds *into);

#endif

#endif
