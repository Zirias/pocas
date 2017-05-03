#ifndef CONTAINER_INTERNAL_H
#define CONTAINER_INTERNAL_H

#include <pocas/gui/container.h>

int Container_create(void *self);
int Container_setBounds(void *self, Bounds *b);
void *Container_owner(void *self);
void Container_setOwner(void *self, void *owner);
void Container_destroy(void *self);

#endif
