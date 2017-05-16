#include <stdlib.h>
#include <string.h>
#include <pocas/core/string.h>

#include <pocas/core/list.h>

struct PC_List
{
    void **items;
    int itercount;
    int disposed;
    size_t capacity;
    size_t length;
    PC_ListElementDeleter deleter;
    PC_ListElementCloner cloner;
};

struct PC_ListIterator
{
    const PC_List *list;
    size_t index;
};

static void checkAndgrow(PC_List *self, size_t index)
{
    if (index >= self->capacity)
    {
        do { self->capacity *= 2; } while (index >= self->capacity);
        self->items = realloc(self->items, self->capacity * sizeof(void *));
    }
}

SOEXPORT PC_List *PC_List_create(size_t capacity, PC_ListElementDeleter deleter,
                           PC_ListElementCloner cloner)
{
    if (!capacity) capacity = 32;
    PC_List *self = malloc(sizeof(PC_List));
    self->items = malloc(capacity * sizeof(void *));
    self->itercount = 0;
    self->disposed = 0;
    self->capacity = capacity;
    self->length = 0;
    self->deleter = deleter;
    self->cloner = cloner;
    return self;
}

SOEXPORT PC_List *PC_List_createStr(size_t capacity)
{
    return PC_List_create(capacity, free, (PC_ListElementCloner)PC_String_copy);
}

SOEXPORT void PC_List_append(PC_List *self, void *item)
{
    size_t index = self->length;
    checkAndgrow(self, index);
    self->items[index] = item;
    ++self->length;
}

SOEXPORT void PC_List_set(PC_List *self, size_t index, void *item)
{
    checkAndgrow(self, index);
    if (index >= self->length)
    {
        for (size_t i = self->length; i < index; ++i)
        {
            self->items[i] = 0;
        }
        self->length = index + 1;
    }
    else if (self->deleter)
    {
        self->deleter(self->items[index]);
    }
    self->items[index] = item;
}

SOEXPORT void PC_List_insert(PC_List *self, size_t index, void *item)
{
    if (index >= self->length)
    {
        PC_List_set(self, index, item);
    }
    else
    {
        checkAndgrow(self, self->length);
        memmove(self->items + index + 1, self->items + index,
                (self->length - index) * sizeof(void *));
        self->items[index] = item;
        ++self->length;
    }
}

SOEXPORT void PC_List_remove(PC_List *self, void *item)
{
    for (size_t i = 0; i < self->length; ++i)
    {
        if (self->items[i] == item) PC_List_removeAt(self, i);
    }
}

SOEXPORT void PC_List_removeAt(PC_List *self, size_t index)
{
    if (self->deleter)
    {
        self->deleter(self->items[index]);
    }
    memmove(self->items + index, self->items + index + 1,
            (self->length - index - 1) * sizeof(void *));
    --self->length;
}

SOEXPORT void PC_List_removeMatching(
        PC_List *self, PC_ListElementSelector selector, void *selectArgs)
{
    for (size_t i = 0; i < self->length; ++i)
    {
        while (i < self->length && selector(self->items[i], selectArgs))
        {
            PC_List_removeAt(self, i);
        }
    }
}

SOEXPORT void *PC_List_get(const PC_List *self, size_t index)
{
    if (index >= self->length) return 0;
    return self->items[index];
}

SOEXPORT char *PC_List_getStr(const PC_List *self, size_t index)
{
    return PC_List_get(self, index);
}

SOEXPORT size_t PC_List_length(const PC_List *self)
{
    return self->length;
}

SOEXPORT void PC_List_clear(PC_List *self)
{
    if (self->deleter)
    {
        for (size_t i = 0; i < self->length; ++i)
        {
            self->deleter(self->items[i]);
        }
    }
    self->length = 0;
}

SOEXPORT PC_List *PC_List_clone(const PC_List *self)
{
    PC_List *result = PC_List_create(0, self->deleter, self->cloner);
    for (size_t i = 0; i < self->length; ++i)
    {
        void *item = self->items[i];
        if (self->cloner) item = self->cloner(item);
        PC_List_append(result, item);
    }
    return result;
}

SOEXPORT void *PC_List_join(const PC_List *self, PC_ListElementSize getElementSize,
                         const void *delim, size_t delimSize,
                         size_t *resultSize,
                         size_t reserveStart, size_t reserveEnd)
{
    if (self->length == 0) return 0;
    size_t *sizes = malloc(self->length * sizeof(size_t));
    size_t n = (self->length - 1) * delimSize + reserveStart + reserveEnd;
    for (size_t i = 0; i < self->length; ++i)
    {
        sizes[i] = getElementSize(self->items[i]);
        n+= sizes[i];
    }
    char *result = calloc(1, n);
    char *wp = result + reserveStart;
    for (size_t i = 0; i < self->length; ++i)
    {
        if (i)
        {
            memcpy(wp, delim, delimSize);
            wp += delimSize;
        }
        memcpy(wp, self->items[i], sizes[i]);
        wp += sizes[i];
    }
    free(sizes);
    if (resultSize) *resultSize = n;
    return result;
}

SOEXPORT char *PC_List_joinStr(const PC_List *self, const char *delim)
{
    return PC_List_join(self, (PC_ListElementSize)strlen, delim, strlen(delim),
                     0, 0, 1);
}

SOEXPORT PC_List *PC_List_transform(const PC_List *self, PC_ListElementDeleter deleter,
                              PC_ListElementCloner cloner,
                              PC_ListElementTransform transformElement,
                              void *transformArgs)
{
    PC_List *result = PC_List_create(0, deleter, cloner);
    for (size_t i = 0; i < self->length; ++i)
    {
        PC_List_append(result, transformElement(self->items[i], transformArgs));
    }
    return result;
}

SOEXPORT PC_List *PC_List_transformSameType(const PC_List *self,
                                 PC_ListElementTransform transformElement,
                                 void *transformArgs)
{
    return PC_List_transform(self, self->deleter, self->cloner,
                          transformElement, transformArgs);
}

SOEXPORT PC_List *PC_List_concat(const PC_List *self, const PC_List *append)
{
    PC_List *result = PC_List_clone(self);
    for (size_t i = 0; i < append->length; ++i)
    {
        void *item = append->items[i];
        if (self->cloner) item = self->cloner(item);
        PC_List_append(result, item);
    }
    return result;
}

SOEXPORT PC_List *PC_List_select(const PC_List *self, PC_ListElementSelector selector,
                           void *selectArgs)
{
    PC_List *result = PC_List_create(0, self->deleter, self->cloner);
    for (size_t i = 0; i < self->length; ++i)
    {
        if (selector(self->items[i], selectArgs))
        {
            void *item = self->items[i];
            if (self->cloner) item = self->cloner(item);
            PC_List_append(result, item);
        }
    }
    return result;
}

SOEXPORT PC_ListIterator *PC_List_iterator(const PC_List *self)
{
    PC_ListIterator *iter = malloc(sizeof(PC_ListIterator));
    iter->list = self;
    ((PC_List *)self)->itercount++;
    iter->index = -1;
    return iter;
}

SOEXPORT int PC_ListIterator_moveNext(PC_ListIterator *self)
{
    if (++self->index >= self->list->length)
    {
        self->index = -1;
        return 0;
    }
    return 1;
}

SOEXPORT void *PC_ListIterator_current(const PC_ListIterator *self)
{
    if (self->index < self->list->length)
    {
        return self->list->items[self->index];
    }
    return 0;
}

SOEXPORT char *PC_ListIterator_currentStr(const PC_ListIterator *self)
{
    return PC_ListIterator_current(self);
}

SOEXPORT void PC_ListIterator_destroy(PC_ListIterator *self)
{
    if (!self) return;
    if (!--((PC_List *)self->list)->itercount && self->list->disposed)
    {
        PC_List_destroy((PC_List *)self->list);
    }
    free(self);
}

SOEXPORT void PC_List_destroy(PC_List *self)
{
    if (!self) return;
    if (self->itercount)
    {
        self->disposed = 1;
    }
    else
    {
        if (self->deleter)
        {
            for (size_t i = 0; i < self->length; ++i)
            {
                self->deleter(self->items[i]);
            }
        }
        free(self->items);
        free(self);
    }
}
