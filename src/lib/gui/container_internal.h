#ifndef CONTAINER_INTERNAL_H
#define CONTAINER_INTERNAL_H

#include <pocas/gui/container.h>

int PG_Container_create(void *self);
int PG_Container_setBounds(void *self, PG_Bounds *b);
void *PG_Container_owner(void *self);
void PG_Container_setOwner(void *self, void *owner);
void PG_Container_destroy(void *self);

#endif
