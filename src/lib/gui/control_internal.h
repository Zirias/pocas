#ifndef CONTROL_INTERNAL_H
#define CONTROL_INTERNAL_H

#include <pocas/gui/control.h>

int Control_create(void* self);
void *Control_container(const void *self);
void Control_setContainer(void *self, void *container);
void Control_setContentSize(void *self,
        unsigned int width, unsigned int height);
void Control_destroy(void *self);

#endif
