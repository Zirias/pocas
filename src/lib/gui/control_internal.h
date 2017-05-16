#ifndef CONTROL_INTERNAL_H
#define CONTROL_INTERNAL_H

#include <pocas/gui/control.h>

int PG_Control_create(void* self);
void *PG_Control_container(const void *self);
void PG_Control_setContainer(void *self, void *container);
void PG_Control_setContentSize(void *self,
        unsigned int width, unsigned int height);
void PG_Control_destroy(void *self);

#endif
