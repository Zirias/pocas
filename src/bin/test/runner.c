#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <pocas/core/list.h>
#include <pocas/core/plugin.h>
#include <pocas/core/textcolor.h>

#include "testmethod_internal.h"
#include "runner_internal.h"

SOLOCAL char *exeName;

SOLOCAL void Runner_runTest(FILE *output, const PC_Plugin *testPlugin,
	const char *methodName)
{
    char *methodSymbol = malloc(19 + strlen(methodName));
    strcpy(methodSymbol, "pocastest__method_");
    strcpy(methodSymbol + 18, methodName);
    void (*testMethod)(PT_TestMethod *) = (void (*)(PT_TestMethod *))
            (uintptr_t)PC_Plugin_symbol(testPlugin, methodSymbol);
    free(methodSymbol);

    if (!testMethod)
    {
        fprintf(output, "0Error in test `%s': test method `%s' not found.\n",
                PC_Plugin_id(testPlugin), methodName);
        goto done;
    }

    void (*testInit)(PT_TestMethod *) = (void (*)(PT_TestMethod *))
            (uintptr_t)PC_Plugin_symbol(testPlugin, "pocastest__init");
    void (*testDone)(PT_TestMethod *) = (void (*)(PT_TestMethod *))
            (uintptr_t)PC_Plugin_symbol(testPlugin, "pocastest__done");

    PT_TestMethod *t = PT_TestMethod_create(output);
    if (!setjmp(*PT_TestMethod_jmp(t)))
    {
        if (testInit) testInit(t);
        testMethod(t);
    }
    if (testDone) testDone(t);
    PT_TestMethod_destroy(t);

done:
    fclose(output);
    exit(EXIT_SUCCESS);
}
