#include <pocas/gui/bounds.h>

SOEXPORT void PG_Bounds_fitInto(PG_Bounds *self, const PG_Bounds *into)
{
        if (self->width > into->width)
        {
            self->width = into->width;
        }
        if (self->height > into->height)
        {
            self->height = into->height;
        }
        if (self->x < into->x)
        {
            self->x = into->x;
        }
        else if (self->x + self->width > into->x + into->width)
        {
            self->x = into->x + into->width - self->width;
        }
        if (self->y < into->y)
        {
            self->y = into->y;
        }
        else if (self->y + self->height > into->y + into->height)
        {
            self->y = into->y + into->height - self->height;
        }
}
