#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <pocas/core/list.h>
#include <pocas/core/plugin.h>
#include <pocas/core/textcolor.h>

#include "testmethod_internal.h"
#include "runner_internal.h"

SOLOCAL char *exeName;

SOLOCAL void Runner_runTest(FILE *output, Plugin *testPlugin, const char *methodName)
{
    char *methodSymbol = malloc(19 + strlen(methodName));
    strcpy(methodSymbol, "pocastest__method_");
    strcpy(methodSymbol + 18, methodName);
    void (*testMethod)(TestMethod *) = (void (*)(TestMethod *))
            (uintptr_t)Plugin_symbol(testPlugin, methodSymbol);
    free(methodSymbol);

    if (!testMethod)
    {
        fprintf(output, "0Error in test `%s': test method `%s' not found.\n",
                Plugin_id(testPlugin), methodName);
        goto done;
    }

    void (*testInit)(TestMethod *) = (void (*)(TestMethod *))
            (uintptr_t)Plugin_symbol(testPlugin, "pocastest__init");
    void (*testDone)(TestMethod *) = (void (*)(TestMethod *))
            (uintptr_t)Plugin_symbol(testPlugin, "pocastest__done");

    TestMethod *t = TestMethod_create(output);
    if (!setjmp(*TestMethod_jmp(t)))
    {
        if (testInit) testInit(t);
        testMethod(t);
    }
    if (testDone) testDone(t);
    TestMethod_destroy(t);

done:
    Plugin_close(testPlugin);
    fclose(output);
    exit(EXIT_SUCCESS);
}
