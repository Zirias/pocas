#ifndef POCAS_GUI_CONTAINER_H
#define POCAS_GUI_CONTAINER_H

#include <pocas/gui/decl.h>

typedef struct Bounds Bounds;
typedef struct Event Event;

DECLEXPORT void *Container_control(const void *self);
DECLEXPORT void Container_setControl(void *self, void *control);
DECLEXPORT void Container_bounds(const void *self, Bounds *b);
DECLEXPORT Event *Container_resizedEvent(const void *self);

#endif
