#ifndef POCAS_CORE_MTWAITQUEUE_H
#define POCAS_CORE_MTWAITQUEUE_H

#include <pocas/core/decl.h>

typedef struct MtWaitQueue MtWaitQueue;

typedef void (*MtWaitQueueElementDeleter)(void *element);

DECLEXPORT MtWaitQueue *MtWaitQueue_create(MtWaitQueueElementDeleter deleter);

DECLEXPORT void MtWaitQueue_enqueue(MtWaitQueue *self, void *element);
DECLEXPORT void *MtWaitQueue_dequeue(MtWaitQueue *self, int timeout);

DECLEXPORT void MtWaitQueue_destroy(MtWaitQueue *self);

#endif
