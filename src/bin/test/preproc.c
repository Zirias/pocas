#include <stdio.h>
#include <string.h>

#include <pocas/core/list.h>
#include <pocas/core/string.h>

#include <pocas/test/decl.h>

#include "preproc_internal.h"

static void skipWs(char **p)
{
    while (**p &&
           (**p == ' ' || **p == '\t' || **p == '\r' || **p == '\n'))
    {
        ++*p;
    }
}

static char *findTestMethod(char *s)
{
    static const char *signature = "PT_TESTMETHOD";

    while (*s)
    {
        skipWs(&s);
        if (!strncmp(s, signature, 13))
        {
            char *f = s + 13;
            skipWs(&f);
            if (*f == '(')
            {
                ++f;
                skipWs(&f);
                if (*f)
                {
                    char *e = f + 1;
                    while (*e && *e != ' ' && *e != '\t' && *e != '\r'
                           && *e != '\n' && *e != ')')
                    {
                        ++e;
                    }
                    if (*e)
                    {
                        if (*e == ')')
                        {
                            *e = '\0';
                            return f;
                        }
                        *e++ = '\0';
                        skipWs(&e);
                        if (*e == ')') return f;
                    }
                }
            }
        }
        ++s;
    }
    return 0;
}

SOLOCAL int preproc(const char *inFileName, const char *outFileName)
{
    FILE *in = fopen(inFileName, "r");
    if (!in) return 0;

    FILE *out = outFileName ? fopen(outFileName, "w") : stdout;
    if (!out)
    {
        fclose(in);
        return 0;
    }

    char buf[1024];
    PC_List *testMethods = PC_List_createStr(0);
    while (fgets(buf, 1024, in))
    {
        char *method = findTestMethod(buf);
        if (method) PC_List_append(testMethods, PC_String_copy(method));
    }

    PC_ListIterator *i = PC_List_iterator(testMethods);
    fputs("\n\n// pocastest -p generated:\n\n#include <pocas/test/test.h>\n\n"
          "SOEXPORT const char *pocastest__methods[] = {", out);
    while (PC_ListIterator_moveNext(i))
    {
        fprintf(out, "\"%s\",",(char *)PC_ListIterator_current(i));
    }
    fputs("0};\n\n", out);

    if (out != stdout) fclose(out);
    fclose(in);
    return 1;
}
