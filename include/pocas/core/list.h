#ifndef POCAS_CORE_LIST_H
#define POCAS_CORE_LIST_H

#include <stddef.h>

#include <pocas/core/decl.h>

typedef struct List List;
typedef struct ListIterator ListIterator;
typedef void (*ListElementDeleter)(void *element);
typedef void *(*ListElementCloner)(const void *element);
typedef size_t (*ListElementSize)(const void *element);
typedef void *(*ListElementTransform)(const void *element, void *args);
typedef int (*ListElementSelector)(const void *element, void *args);

DECLEXPORT List *List_create(size_t capacity, ListElementDeleter deleter,
                             ListElementCloner cloner);
DECLEXPORT List *List_createStr(size_t capacity);
DECLEXPORT void List_append(List *self, void *item);
DECLEXPORT void List_set(List *self, size_t index, void *item);
DECLEXPORT void List_insert(List *self, size_t index, void *item);
DECLEXPORT void List_remove(List *self, void *item);
DECLEXPORT void List_removeAt(List *self, size_t index);
DECLEXPORT void List_removeMatching(List *self, ListElementSelector selector,
                                    void *selectArgs);
DECLEXPORT void *List_get(const List *self, size_t index);
DECLEXPORT char *List_getStr(const List *self, size_t index);
DECLEXPORT size_t List_length(const List *self);
DECLEXPORT void List_clear(List *self);
DECLEXPORT List *List_clone(const List *self);
DECLEXPORT void *List_join(const List *self, ListElementSize getElementSize,
                           const void *delim, size_t delimSize,
                           size_t *resultSize,
                           size_t reserveStart, size_t reserveEnd);
DECLEXPORT char *List_joinStr(const List *self, const char *delim);
DECLEXPORT List *List_transform(const List *self, ListElementDeleter deleter,
                                ListElementCloner cloner,
                                ListElementTransform transformElement,
                                void *transformArgs);
DECLEXPORT List *List_transformSameType(const List *self,
                                   ListElementTransform transformElement,
                                   void *transformArgs);
DECLEXPORT List *List_concat(const List *self, const List *append);
DECLEXPORT List *List_select(const List *self, ListElementSelector selector,
                             void *selectArgs);
DECLEXPORT ListIterator *List_iterator(const List *self);
DECLEXPORT int ListIterator_moveNext(ListIterator *self);
DECLEXPORT void *ListIterator_current(const ListIterator *self);
DECLEXPORT char *ListIterator_currentStr(const ListIterator *self);
DECLEXPORT void ListIterator_destroy(ListIterator *self);
DECLEXPORT void List_destroy(List *self);

#endif
