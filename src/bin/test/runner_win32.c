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

SOLOCAL void Runner_mainHook(PC_List *args, char *gdbPath)
{
    if (!PC_List_length(args)) return;
    if (strcmp("__pocastest__run", PC_List_getStr(args, 0))) return;

    if (gdbPath)
    {
        int i = 500;
        while (--i && !IsDebuggerPresent()) Sleep(10);
    }

    if (PC_List_length(args) != 4) exit(EXIT_FAILURE);

    uintptr_t testPipeHandleValue;
    if (sscanf(PC_List_getStr(args, 1), "%" SCNxPTR, &testPipeHandleValue) != 1)
    {
        exit(EXIT_FAILURE);
    }

    int testPipeFd = _open_osfhandle(
                (intptr_t)testPipeHandleValue, _O_APPEND | _O_WRONLY);
    FILE *testPipe = _fdopen(testPipeFd, "a");
    setvbuf(testPipe, 0, _IONBF, 0);

    PC_Plugin *runningTest = PC_Plugin_load(PC_List_getStr(args, 2), TEST_PLUGIN_ID);
    if (!runningTest)
    {
        fprintf(testPipe, "0Error loading test `%s'.i\n", PC_List_getStr(args, 2));
        fclose(testPipe);
        exit(EXIT_FAILURE);
    }

    SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
    Runner_runTest(testPipe, runningTest, PC_List_getStr(args, 3));
}
