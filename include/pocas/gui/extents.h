#ifndef POCAS_GUI_EXTENTS_H
#define POCAS_GUI_EXTENTS_H

#include <pocas/gui/point.h>
#include <pocas/gui/bounds.h>

C_CLASS_DECL(PG_Extents);
#ifdef __cplusplus
struct PG_Extents
{
    PG_Point topLeft;
    PG_Point bottomRight;
};
#else
struct PG_Extents
{
    union
    {
        struct
        {
            PG_Point topLeft;
            PG_Point bottomRight;
        };
        struct
        {
            struct
            {
                int left;
                int top;
            };
            struct
            {
                int right;
                int bottom;
            };
        };
    };
};
#endif

#define PG_Extents_invalidExtents \
    {.topLeft=PG_Point_invalidPoint, .bottomRight=PG_Point_invalidPoint}

#define PG_Extents_valid(e) (PG_Point_valid(&((e)->topLeft)) \
    && PG_Point_valid(&((e)->bottomRight)))
#define PG_Extents_setInvalid(e) do { \
    PG_Point_setInvalid(&((e)->topLeft)); \
    PG_Point_setInvalid(&((e)->bottomRight)); \
    } while (0)
#define PG_Extents_equals(e, other) \
    (PG_Point_equals(&((e)->topLeft), &((other)->topLeft)) \
    && PG_Point_equals(&((e)->bottomRight), &((other)->bottomRight)))
#define PG_Extents_toBounds(e, b) do { \
    (b)->topLeft.x = (e)->topLeft.x; \
    (b)->topLeft.y = (e)->topLeft.y; \
    (b)->size.width = (e)->bottomRight.x - (e)->topLeft.x; \
    (b)->size.height = (e)->bottomRight.y - (e)->topLeft.y; \
    } while (0)

#define PG_Extents_init(ileft, itop, iright, ibottom) { \
    .topLeft=PG_Point_init((ileft),(itop)), \
    .bottomRight=PG_Point_init((iright),(ibottom)) }

#endif
