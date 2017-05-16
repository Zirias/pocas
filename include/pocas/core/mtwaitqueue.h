#ifndef POCAS_CORE_MTWAITQUEUE_H
#define POCAS_CORE_MTWAITQUEUE_H

#include <pocas/core/decl.h>

C_CLASS_DECL(PC_MtWaitQueue);

typedef void (*PC_MtWaitQueueElementDeleter)(void *element);

DECLEXPORT PC_MtWaitQueue *PC_MtWaitQueue_create(PC_MtWaitQueueElementDeleter deleter);

DECLEXPORT void PC_MtWaitQueue_enqueue(PC_MtWaitQueue *self, void *element);
DECLEXPORT void *PC_MtWaitQueue_dequeue(PC_MtWaitQueue *self, int timeout);

DECLEXPORT void PC_MtWaitQueue_destroy(PC_MtWaitQueue *self);

#endif
