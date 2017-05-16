#ifndef POCAS_CORE_LIST_H
#define POCAS_CORE_LIST_H

#include <stddef.h>

#include <pocas/core/decl.h>

C_CLASS_DECL(PC_List);
C_CLASS_DECL(PC_ListIterator);

typedef void (*PC_ListElementDeleter)(void *element);
typedef void *(*PC_ListElementCloner)(const void *element);
typedef size_t (*PC_ListElementSize)(const void *element);
typedef void *(*PC_ListElementTransform)(const void *element, void *args);
typedef int (*PC_ListElementSelector)(const void *element, void *args);

DECLEXPORT PC_List *PC_List_create(size_t capacity, PC_ListElementDeleter deleter,
                             PC_ListElementCloner cloner);
DECLEXPORT PC_List *PC_List_createStr(size_t capacity);
DECLEXPORT void PC_List_append(PC_List *self, void *item);
DECLEXPORT void PC_List_set(PC_List *self, size_t index, void *item);
DECLEXPORT void PC_List_insert(PC_List *self, size_t index, void *item);
DECLEXPORT void PC_List_remove(PC_List *self, void *item);
DECLEXPORT void PC_List_removeAt(PC_List *self, size_t index);
DECLEXPORT void PC_List_removeMatching(PC_List *self, PC_ListElementSelector selector,
                                    void *selectArgs);
DECLEXPORT void *PC_List_get(const PC_List *self, size_t index);
DECLEXPORT char *PC_List_getStr(const PC_List *self, size_t index);
DECLEXPORT size_t PC_List_length(const PC_List *self);
DECLEXPORT void PC_List_clear(PC_List *self);
DECLEXPORT PC_List *PC_List_clone(const PC_List *self);
DECLEXPORT void *PC_List_join(const PC_List *self, PC_ListElementSize getElementSize,
                           const void *delim, size_t delimSize,
                           size_t *resultSize,
                           size_t reserveStart, size_t reserveEnd);
DECLEXPORT char *PC_List_joinStr(const PC_List *self, const char *delim);
DECLEXPORT PC_List *PC_List_transform(const PC_List *self, PC_ListElementDeleter deleter,
                                PC_ListElementCloner cloner,
                                PC_ListElementTransform transformElement,
                                void *transformArgs);
DECLEXPORT PC_List *PC_List_transformSameType(const PC_List *self,
                                   PC_ListElementTransform transformElement,
                                   void *transformArgs);
DECLEXPORT PC_List *PC_List_concat(const PC_List *self, const PC_List *append);
DECLEXPORT PC_List *PC_List_select(const PC_List *self, PC_ListElementSelector selector,
                             void *selectArgs);
DECLEXPORT PC_ListIterator *PC_List_iterator(const PC_List *self);
DECLEXPORT int PC_ListIterator_moveNext(PC_ListIterator *self);
DECLEXPORT void *PC_ListIterator_current(const PC_ListIterator *self);
DECLEXPORT char *PC_ListIterator_currentStr(const PC_ListIterator *self);
DECLEXPORT void PC_ListIterator_destroy(PC_ListIterator *self);
DECLEXPORT void PC_List_destroy(PC_List *self);

#endif
