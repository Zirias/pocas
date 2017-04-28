#ifndef POCAS_GUI_CONTAINER_H
#define POCAS_GUI_CONTAINER_H

#include <pocas/gui/decl.h>

typedef struct Event Event;

DECLEXPORT void *Container_control(const void *self);
DECLEXPORT void Container_setControl(void *self, void *control);
DECLEXPORT unsigned int Container_width(const void *self);
DECLEXPORT unsigned int Container_height(const void *self);
DECLEXPORT Event *Container_resizedEvent(const void *self);

#endif
