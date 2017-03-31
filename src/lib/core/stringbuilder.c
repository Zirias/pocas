#include <stdlib.h>
#include <string.h>

#include <pocas/core/stringbuilder.h>

struct StringBuilder
{
    size_t initial_capacity;
    size_t capacity;
    size_t length;
    char *content;
    char *writepos;
};

SOEXPORT StringBuilder *StringBuilder_create(size_t capacity)
{
    if (!capacity) capacity = 256;

    StringBuilder *self = malloc(sizeof(StringBuilder));
    self->initial_capacity = capacity;
    self->content = 0;
    self->length = 0;
    return self;
}

SOEXPORT void StringBuilder_appendChar(StringBuilder *self, char character)
{
    if (!self->content)
    {
        self->content = malloc(self->initial_capacity);
        self->writepos = self->content;
        self->capacity = self->initial_capacity;
        self->length = 0;
    }
    else if (self->length == self->capacity)
    {
        self->capacity *= 2;
        self->content = realloc(self->content, self->capacity);
        self->writepos = self->content + self->length;
    }

    ++self->length;
    *(self->writepos)++ = character;
}

SOEXPORT void StringBuilder_appendStr(StringBuilder *self, const char *string)
{
    char c;
    while ((c = *string++))
    {
        StringBuilder_appendChar(self, c);
    }
}

SOEXPORT void StringBuilder_clear(StringBuilder *self)
{
    free(self->content);
    self->content = 0;
    self->length = 0;
}

SOEXPORT size_t StringBuilder_length(const StringBuilder *self)
{
    return self->length;
}

SOEXPORT char *StringBuilder_toString(const StringBuilder *self)
{
    char *result = malloc(self->length + 1);
    memcpy(result, self->content, self->length);
    result[self->length] = 0;
    return result;
}

SOEXPORT void StringBuilder_destroy(StringBuilder *self)
{
    free(self->content);
    free(self);
}
