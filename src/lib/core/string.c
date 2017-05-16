#include <stdlib.h>
#include <string.h>

#include <pocas/core/string.h>

SOEXPORT char *PC_String_copy(const char *self)
{
    char *result = malloc(strlen(self) + 1);
    char *w = result;
    for (const char *c = self; *c; ++c)
    {
        *w++ = *c;
    }
    *w = 0;
    return result;
}
