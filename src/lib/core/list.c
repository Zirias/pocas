#include <stdlib.h>
#include <string.h>
#include <pocas/core/string.h>

#include <pocas/core/list.h>

struct List
{
    void **items;
    int itercount;
    int disposed;
    size_t capacity;
    size_t length;
    ListElementDeleter deleter;
    ListElementCloner cloner;
};

struct ListIterator
{
    const List *list;
    size_t index;
};

static void checkAndgrow(List *self, size_t index)
{
    if (index >= self->capacity)
    {
        do { self->capacity *= 2; } while (index >= self->capacity);
        self->items = realloc(self->items, self->capacity * sizeof(void *));
    }
}

SOEXPORT List *List_create(size_t capacity, ListElementDeleter deleter,
                           ListElementCloner cloner)
{
    if (!capacity) capacity = 32;
    List *self = malloc(sizeof(List));
    self->items = malloc(capacity * sizeof(void *));
    self->itercount = 0;
    self->disposed = 0;
    self->capacity = capacity;
    self->length = 0;
    self->deleter = deleter;
    self->cloner = cloner;
    return self;
}

SOEXPORT List *List_createStr(size_t capacity)
{
    return List_create(capacity, free, (ListElementCloner)String_copy);
}

SOEXPORT void List_append(List *self, void *item)
{
    size_t index = self->length;
    checkAndgrow(self, index);
    self->items[index] = item;
    ++self->length;
}

SOEXPORT void List_set(List *self, size_t index, void *item)
{
    checkAndgrow(self, index);
    self->items[index] = item;
    if (index >= self->length)
    {
        for (size_t i = self->length; i < index; ++i)
        {
            self->items[i] = 0;
        }
        self->length = index + 1;
    }
}

SOEXPORT void List_insert(List *self, size_t index, void *item)
{
    if (index >= self->length)
    {
        List_set(self, index, item);
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

SOEXPORT void List_remove(List *self, void *item)
{
    for (size_t i = 0; i < self->length; ++i)
    {
        if (self->items[i] == item) List_removeAt(self, i);
    }
}

SOEXPORT void List_removeAt(List *self, size_t index)
{
    if (self->deleter)
    {
        self->deleter(self->items[index]);
    }
    memmove(self->items + index, self->items + index + 1,
            (self->length - index - 1) * sizeof(void *));
    --self->length;
}

SOEXPORT void List_removeMatching(
        List *self, ListElementSelector selector, void *selectArgs)
{
    for (size_t i = 0; i < self->length; ++i)
    {
        while (i < self->length && selector(self->items[i], selectArgs))
        {
            List_removeAt(self, i);
        }
    }
}

SOEXPORT void *List_get(const List *self, size_t index)
{
    if (index >= self->length) return 0;
    return self->items[index];
}

SOEXPORT char *List_getStr(const List *self, size_t index)
{
    return List_get(self, index);
}

SOEXPORT size_t List_length(const List *self)
{
    return self->length;
}

SOEXPORT void List_clear(List *self)
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

SOEXPORT List *List_clone(const List *self)
{
    List *result = List_create(0, self->deleter, self->cloner);
    for (size_t i = 0; i < self->length; ++i)
    {
        void *item = self->items[i];
        if (self->cloner) item = self->cloner(item);
        List_append(result, item);
    }
    return result;
}

SOEXPORT void *List_join(const List *self, ListElementSize getElementSize,
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

SOEXPORT char *List_joinStr(const List *self, const char *delim)
{
    return List_join(self, (ListElementSize)strlen, delim, strlen(delim),
                     0, 0, 1);
}

SOEXPORT List *List_transform(const List *self, ListElementDeleter deleter,
                              ListElementCloner cloner,
                              ListElementTransform transformElement,
                              void *transformArgs)
{
    List *result = List_create(0, deleter, cloner);
    for (size_t i = 0; i < self->length; ++i)
    {
        List_append(result, transformElement(self->items[i], transformArgs));
    }
    return result;
}

SOEXPORT List *List_transformSameType(const List *self,
                                 ListElementTransform transformElement,
                                 void *transformArgs)
{
    return List_transform(self, self->deleter, self->cloner,
                          transformElement, transformArgs);
}

SOEXPORT List *List_concat(const List *self, const List *append)
{
    List *result = List_clone(self);
    for (size_t i = 0; i < append->length; ++i)
    {
        void *item = append->items[i];
        if (self->cloner) item = self->cloner(item);
        List_append(result, item);
    }
    return result;
}

SOEXPORT List *List_select(const List *self, ListElementSelector selector,
                           void *selectArgs)
{
    List *result = List_create(0, self->deleter, self->cloner);
    for (size_t i = 0; i < self->length; ++i)
    {
        if (selector(self->items[i], selectArgs))
        {
            void *item = self->items[i];
            if (self->cloner) item = self->cloner(item);
            List_append(result, item);
        }
    }
    return result;
}

SOEXPORT ListIterator *List_iterator(const List *self)
{
    ListIterator *iter = malloc(sizeof(ListIterator));
    iter->list = self;
    ((List *)self)->itercount++;
    iter->index = -1;
    return iter;
}

SOEXPORT int ListIterator_moveNext(ListIterator *self)
{
    if (++self->index >= self->list->length)
    {
        self->index = -1;
        return 0;
    }
    return 1;
}

SOEXPORT void *ListIterator_current(const ListIterator *self)
{
    if (self->index < self->list->length)
    {
        return self->list->items[self->index];
    }
    return 0;
}

SOEXPORT char *ListIterator_currentStr(const ListIterator *self)
{
    return ListIterator_current(self);
}

SOEXPORT void ListIterator_destroy(ListIterator *self)
{
    if (!--((List *)self->list)->itercount && self->list->disposed)
    {
        List_destroy((List *)self->list);
    }
    free(self);
}

SOEXPORT void List_destroy(List *self)
{
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
