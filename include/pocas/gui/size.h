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

#define PG_Size_invalidHeight INT_MIN
#define PG_Size_invalidWidth INT_MIN
#ifdef __cplusplus
#define PG_Size_invalidSize \
    {PG_Size_invalidWidth, PG_Size_invalidHeight}
#else
#define PG_Size_invalidSize \
    {.width=PG_Size_invalidWidth, .height=PG_Size_invalidHeight}
#endif

#define PG_Size_widthValid(s) ((s)->width != PG_Size_invalidWidth)
#define PG_Size_heightValid(s) ((s)->height != PG_Size_invalidHeight)
#define PG_Size_valid(s) (PG_Size_widthValid((s)) && PG_Size_heightValid((s)))
#define PG_Size_setInvalid(s) do { \
    (s)->width = PG_Size_invalidWidth; \
    (s)->height = PG_Size_invalidHeight; \
    } while (0)
#define PG_Size_equals(s, other) \
    ((s)->width == (other)->width && (s)->height == (other)->height)

#ifdef __cplusplus
#define PG_Size_init(iwidth, iheight) {(iwidth), (iheight)}
#else
#define PG_Size_init(iwidth, iheight) {.width=(iwidth), .height=(iheight)}
#endif

#endif
