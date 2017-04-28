#ifndef CONTAINER_INTERNAL_H
#define CONTAINER_INTERNAL_H

#include <pocas/gui/container.h>

int Container_create(void *self);
void Container_setWidth(void *self, unsigned int width);
void Container_setHeight(void *self, unsigned int height);
void Container_destroy(void *self);

#endif
