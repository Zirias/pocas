#include <pocas/gui/bounds.h>

SOEXPORT int Bounds_equals(const Bounds *self, const Bounds *other)
{
    return (self->height == other->height
            && self->width == other->width
            && self->x == other->x
            && self->y == other->y);
}

SOEXPORT void Bounds_fitInto(Bounds *self, const Bounds *into)
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
