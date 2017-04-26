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

#include "runner_internal.h"
#include "testclass_internal.h"
#include "testcase_internal.h"

static void stripNewline(char *s)
{
    while (*s)
    {
        if (*s == '\n')
        {
            *s = '\0';
            return;
        }
        ++s;
    }
}

void TestCase_run(TestCase *self, const char *gdbPath)
{
    char buf[1024];
    HANDLE pin, pout;
    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(sa);
    sa.lpSecurityDescriptor = 0;
    sa.bInheritHandle = 1;

    if (!CreatePipe(&pin, &pout, &sa, 0))
    {
        fprintf(stderr, "Error creating pipe: %lu\n", GetLastError());
        return;
    }
    int runnerPipeFd = _open_osfhandle((intptr_t)pin, _O_APPEND | _O_RDONLY);
    FILE *runnerPipe = _fdopen(runnerPipeFd, "r");
    setvbuf(runnerPipe, 0, _IONBF, 0);

    HANDLE nul = CreateFile("nul", GENERIC_READ, 0, &sa, 0, 0, 0);

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput = nul;
    si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

    snprintf(buf, 1024, "%" PRIxPTR, (uintptr_t)pout);

    StringBuilder *cmdLineBuilder = StringBuilder_create(0);
    StringBuilder_appendChar(cmdLineBuilder, '"');
    StringBuilder_appendStr(cmdLineBuilder, exeName);
    if (gdbPath)
    {
        StringBuilder_appendStr(cmdLineBuilder, "\" -g \"");
        StringBuilder_appendStr(cmdLineBuilder, gdbPath);
    }
    StringBuilder_appendStr(cmdLineBuilder, "\" __pocastest__run ");
    StringBuilder_appendStr(cmdLineBuilder, buf);
    StringBuilder_appendStr(cmdLineBuilder, " \"");
    StringBuilder_appendStr(cmdLineBuilder, Plugin_path(TestClass_plugin(TestCase_class(self))));
    StringBuilder_appendStr(cmdLineBuilder, "\" ");
    StringBuilder_appendStr(cmdLineBuilder, TestCase_method(self));
    char *cmdline = StringBuilder_toString(cmdLineBuilder);
    StringBuilder_destroy(cmdLineBuilder);

    if (!CreateProcess(0, cmdline, 0, 0, 1, 0, 0, 0, &si, &pi))
    {
        free(cmdline);
        CloseHandle(pin);
        CloseHandle(pout);
        fprintf(stderr, "Error creating child process: %lu\n",
                GetLastError());
        return;
    }
    free(cmdline);
    CloseHandle(pout);

    if (gdbPath)
    {
        STARTUPINFO gsi;
        PROCESS_INFORMATION gpi;

        memset(&gsi, 0, sizeof(gsi));
        gsi.cb = sizeof(gsi);
        gsi.dwFlags |= STARTF_USESTDHANDLES;

        cmdLineBuilder = StringBuilder_create(0);
        StringBuilder_appendChar(cmdLineBuilder, '"');
        StringBuilder_appendStr(cmdLineBuilder, gdbPath);
        StringBuilder_appendStr(cmdLineBuilder, "\" -p ");
        StringBuilder_appendUInt(cmdLineBuilder, GetProcessId(pi.hProcess));
        StringBuilder_appendStr(cmdLineBuilder, " -ex \"set breakpoint pending on\" -ex \"break pocastest__method_");
        StringBuilder_appendStr(cmdLineBuilder, TestCase_method(self));
        StringBuilder_appendStr(cmdLineBuilder, "\" -ex c");
        cmdline = StringBuilder_toString(cmdLineBuilder);
        StringBuilder_destroy(cmdLineBuilder);
        if (CreateProcess(0, cmdline, 0, 0, 1, CREATE_NEW_CONSOLE, 0, 0, &gsi, &gpi))
        {
            CloseHandle(gpi.hThread);
            CloseHandle(gpi.hProcess);
        }
        else
        {
            fprintf(stderr, "Error launching gdb: %lu\n",
                    GetLastError());
            fputs("Proceeding without debugging.\n", stderr);
        }
        free(cmdline);
    }

    CloseHandle(pi.hThread);

    char *line;
    List *output = List_createStr(0);
    while ((line = fgets(buf, 1024, runnerPipe)))
    {
        stripNewline(line);
        List_append(output, String_copy(line));
    }

    if (WaitForSingleObject(pi.hProcess, 1000) == WAIT_TIMEOUT)
    {
        TerminateProcess(pi.hProcess, EXIT_FAILURE);
    }
    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hProcess);
    CloseHandle(pin);

    TestCase_evaluate(self, (int)exitCode, output);
}
