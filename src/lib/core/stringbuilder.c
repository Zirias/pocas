#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pocas/core/stringbuilder.h>

struct PC_StringBuilder
{
    size_t initial_capacity;
    size_t capacity;
    size_t length;
    char *content;
    char *writepos;
};

SOEXPORT PC_StringBuilder *PC_StringBuilder_create(size_t capacity)
{
    if (!capacity) capacity = 256;

    PC_StringBuilder *self = malloc(sizeof(PC_StringBuilder));
    self->initial_capacity = capacity;
    self->content = 0;
    self->length = 0;
    return self;
}

SOEXPORT void PC_StringBuilder_appendChar(PC_StringBuilder *self, char character)
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

SOEXPORT void PC_StringBuilder_appendStr(PC_StringBuilder *self, const char *string)
{
    char c;
    while ((c = *string++))
    {
        PC_StringBuilder_appendChar(self, c);
    }
}

SOEXPORT void PC_StringBuilder_appendInt(PC_StringBuilder *self, long value)
{
    char buf[64];
    snprintf(buf, 64, "%ld", value);
    PC_StringBuilder_appendStr(self, buf);
}

SOEXPORT void PC_StringBuilder_appendUInt(PC_StringBuilder *self, unsigned long value)
{
    char buf[64];
    snprintf(buf, 64, "%lu", value);
    PC_StringBuilder_appendStr(self, buf);
}

SOEXPORT void PC_StringBuilder_clear(PC_StringBuilder *self)
{
    free(self->content);
    self->content = 0;
    self->length = 0;
}

SOEXPORT size_t PC_StringBuilder_length(const PC_StringBuilder *self)
{
    return self->length;
}

SOEXPORT char *PC_StringBuilder_toString(const PC_StringBuilder *self)
{
    char *result = malloc(self->length + 1);
    memcpy(result, self->content, self->length);
    result[self->length] = 0;
    return result;
}

SOEXPORT void PC_StringBuilder_destroy(PC_StringBuilder *self)
{
    free(self->content);
    free(self);
}
