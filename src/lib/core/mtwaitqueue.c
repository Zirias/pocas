#include "c11threads.h"

#include <stdlib.h>
#include <time.h>

#include <pocas/core/mtwaitqueue.h>

typedef struct MtWaitQueueEntry MtWaitQueueEntry;

struct MtWaitQueueEntry
{
    MtWaitQueueEntry *prev;
    void *element;
};

struct MtWaitQueue
{
    mtx_t lock;
    cnd_t cv;
    int waiting;
    MtWaitQueueElementDeleter deleter;
    MtWaitQueueEntry *back;
    MtWaitQueueEntry *front;
};

SOEXPORT MtWaitQueue *MtWaitQueue_create(MtWaitQueueElementDeleter deleter)
{
    MtWaitQueue *self = malloc(sizeof(MtWaitQueue));
    if (!self) return 0;
    if (mtx_init(&self->lock, mtx_plain) != 0)
    {
        free(self);
        return 0;
    }
    if (cnd_init(&self->cv) != 0)
    {
        free(self);
        return 0;
    }
    self->waiting = 0;
    self->deleter = deleter;
    self->back = 0;
    self->front = 0;
    return self;
}

SOEXPORT void MtWaitQueue_enqueue(MtWaitQueue *self, void *element)
{
    MtWaitQueueEntry *entry = malloc(sizeof(MtWaitQueueEntry));
    entry->element = element;
    entry->prev = 0;
    mtx_lock(&self->lock);
    if (self->front) self->front->prev = entry;
    self->front = entry;
    mtx_unlock(&self->lock);
    if (self->waiting) cnd_signal(&self->cv);
}

SOEXPORT void *MtWaitQueue_dequeue(MtWaitQueue *self, int timeout)
{
    if (!timeout && !self->front) return 0;
    mtx_lock(&self->lock);
getfront:
    if (self->front)
    {
        MtWaitQueueEntry *front = self->front->prev;
        void *element = self->front->element;
        free(self->front);
        self->front = front;
        mtx_unlock(&self->lock);
        return element;
    }
    if (timeout)
    {
        int err = 0;
        ++self->waiting;
        if (timeout < 0)
        {
            err = cnd_wait(&self->cv, &self->lock);
        }
        else
        {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeout / 1000;
            ts.tv_nsec += (timeout % 1000) * 1000000L;
            if (ts.tv_nsec >= 1000000000L)
            {
                ts.tv_nsec -= 1000000000L;
                ++ts.tv_sec;
            }
            err = cnd_timedwait(&self->cv, &self->lock, &ts);
        }
        --self->waiting;
        if (err == ETIMEDOUT) goto empty;
        goto getfront;
    }
empty:
    mtx_unlock(&self->lock);
    return 0;
}

SOEXPORT void MtWaitQueue_destroy(MtWaitQueue *self)
{
    mtx_lock(&self->lock);
    cnd_destroy(&self->cv);
    mtx_destroy(&self->lock);

    while (self->front)
    {
        MtWaitQueueEntry *front = self->front->prev;
        if (self->deleter) self->deleter(self->front->element);
        free(self->front);
        self->front = front;
    }
    free(self);
}
