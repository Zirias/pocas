#ifndef POCAS_CORE_STRINGBUILDER_H
#define POCAS_CORE_STRINGBUILDER_H

#include <stddef.h>

#include <pocas/core/decl.h>

C_CLASS_DECL(PC_StringBuilder);

DECLEXPORT PC_StringBuilder *PC_StringBuilder_create(size_t capacity);
DECLEXPORT void PC_StringBuilder_appendChar(PC_StringBuilder *self, char character);
DECLEXPORT void PC_StringBuilder_appendStr(PC_StringBuilder *self, const char *string);
DECLEXPORT void PC_StringBuilder_appendInt(PC_StringBuilder *self, long value);
DECLEXPORT void PC_StringBuilder_appendUInt(PC_StringBuilder *self, unsigned long value);
DECLEXPORT void PC_StringBuilder_clear(PC_StringBuilder *self);
DECLEXPORT size_t PC_StringBuilder_length(const PC_StringBuilder *self);
DECLEXPORT char *PC_StringBuilder_toString(const PC_StringBuilder *self);
DECLEXPORT void PC_StringBuilder_destroy(PC_StringBuilder *self);

#endif
