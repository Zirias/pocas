#include <fcntl.h>
#include <inttypes.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <pocas/core/list.h>
#include <pocas/core/plugin.h>
#include <pocas/core/string.h>
#include <pocas/core/stringbuilder.h>

#include "testmethod_internal.h"
#include "runner_internal.h"

SOLOCAL void Runner_mainHook(List *args, char *gdbPath)
{
    if (!List_length(args)) return;
    if (strcmp("__pocastest__run", List_getStr(args, 0))) return;

    if (gdbPath)
    {
        int i = 500;
        while (--i && !IsDebuggerPresent()) Sleep(10);
    }

    if (List_length(args) != 4) exit(EXIT_FAILURE);

    uintptr_t testPipeHandleValue;
    if (sscanf(List_getStr(args, 1), "%" SCNxPTR, &testPipeHandleValue) != 1)
    {
        exit(EXIT_FAILURE);
    }

    int testPipeFd = _open_osfhandle(
                (intptr_t)testPipeHandleValue, _O_APPEND | _O_WRONLY);
    testPipe = _fdopen(testPipeFd, "a");
    setvbuf(testPipe, 0, _IONBF, 0);

    runningTest = Plugin_load(List_getStr(args, 2), TEST_PLUGIN_ID);
    if (!runningTest)
    {
        fprintf(testPipe, "0Error loading test `%s'.i\n", List_getStr(args, 2));
        fclose(testPipe);
        exit(EXIT_FAILURE);
    }

    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    Runner_runTest(List_getStr(args, 3));
}
