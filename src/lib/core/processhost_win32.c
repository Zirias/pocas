#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include <pocas/core/event.h>
#include <pocas/core/string.h>
#include <pocas/core/list.h>
#include <pocas/core/file_win32.h>

#include <pocas/core/processhost.h>

struct PC_ProcessHost
{
    PC_List *executable;
    HANDLE process;
    PC_File *in;
    PC_File *out;
    PC_File *err;
    PC_Event *stdoutData;
    PC_Event *stderrData;
};

static int mkpipe(HANDLE *read, HANDLE *write,
        SECURITY_ATTRIBUTES *rdattr, SECURITY_ATTRIBUTES *wrattr,
        DWORD rdMode, DWORD wrMode)
{
    char name[MAX_PATH];
    static volatile long serial = 0;
    HANDLE rd, wr;
    DWORD size = 4096;

    snprintf(name, MAX_PATH, "\\\\.\\Pipe\\RemoteExeAnon.%08lx.%08lx",
            GetCurrentProcessId(), InterlockedIncrement(&serial));
    rd = CreateNamedPipeA(name, PIPE_ACCESS_INBOUND|rdMode,
            PIPE_TYPE_BYTE|PIPE_WAIT, 1, size, size, 120000, rdattr);
    if (rd == INVALID_HANDLE_VALUE) return 0;
    wr = CreateFileA(name, GENERIC_WRITE, 0, wrattr, OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL|wrMode, 0);
    if (wr == INVALID_HANDLE_VALUE)
    {
        DWORD err = GetLastError();
        CloseHandle(rd);
        SetLastError(err);
        return 0;
    }

    *read = rd;
    *write = wr;
    return 1;
}

static void onStdoutData(void *selfptr, PC_EventArgs *args)
{
    PC_ProcessHost *self = selfptr;
    PC_EventArgs stdoutArgs = PC_EventArgs_init(
                self->stdoutData, self, args->evInfo);
    PC_Event_raise(self->stdoutData, &stdoutArgs);
}

static void onStderrData(void *selfptr, PC_EventArgs *args)
{
    PC_ProcessHost *self = selfptr;
    PC_EventArgs stderrArgs = PC_EventArgs_init(
                self->stderrData, self, args->evInfo);
    PC_Event_raise(self->stderrData, &stderrArgs);
}

SOEXPORT PC_ProcessHost *PC_ProcessHost_create(const char *executable)
{
    PC_ProcessHost *self = malloc(sizeof(PC_ProcessHost));
    self->executable = PC_List_createStr(1);
    PC_List_append(self->executable, PC_String_copy(executable));
    self->process = INVALID_HANDLE_VALUE;
    self->in = 0;
    self->out = 0;
    self->err = 0;
    self->stdoutData = PC_Event_create("Data");
    self->stderrData = PC_Event_create("Data");
    return self;
}

SOEXPORT int PC_ProcessHost_startv(PC_ProcessHost *self, PC_ProcessHostFlags flags, const PC_List *args)
{
    char windir[MAX_PATH];
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    HANDLE nul, in, out, err, pin, pout, perr;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = 1;
    sa.lpSecurityDescriptor = 0;

    if (flags & PC_PH_NO_STDIO)
    {
        nul = CreateFile("nul", GENERIC_READ|GENERIC_WRITE, 0, &sa, 0, 0, 0);
    }
    else
    {
        nul = INVALID_HANDLE_VALUE;
    }
    in = flags & PC_PH_NO_STDIN ? nul : INVALID_HANDLE_VALUE;
    pin = INVALID_HANDLE_VALUE;
    out = flags & PC_PH_NO_STDOUT ? nul : INVALID_HANDLE_VALUE;
    pout = INVALID_HANDLE_VALUE;
    err = flags & PC_PH_NO_STDERR ? nul : INVALID_HANDLE_VALUE;
    perr = INVALID_HANDLE_VALUE;

    if (flags & PC_PH_REDIR_STDIN && in == INVALID_HANDLE_VALUE)
    {
        mkpipe(&in, &pin, &sa, 0, 0, 0);
    }

    if (flags & PC_PH_REDIR_STDOUT && out == INVALID_HANDLE_VALUE)
    {
        mkpipe(&pout, &out, 0, &sa, FILE_FLAG_OVERLAPPED, 0);
    }

    if (flags & PC_PH_REDIR_STDERR && !(flags & PC_PH_REDIR_STDERR_IS_STDOUT)
            && err == INVALID_HANDLE_VALUE)
    {
        mkpipe(&perr, &err, 0, &sa, FILE_FLAG_OVERLAPPED, 0);
    }

    memset(&si, 0, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput = in == INVALID_HANDLE_VALUE ?
                GetStdHandle(STD_INPUT_HANDLE) : in;
    si.hStdOutput = out == INVALID_HANDLE_VALUE ?
                GetStdHandle(STD_OUTPUT_HANDLE) : out;
    si.hStdError = err == INVALID_HANDLE_VALUE ?
                flags & PC_PH_REDIR_STDERR_IS_STDOUT ?
                    si.hStdOutput :
                    GetStdHandle(STD_ERROR_HANDLE)
                  : err;
    si.dwFlags |= STARTF_USESTDHANDLES;

    if (pin != INVALID_HANDLE_VALUE) self->in = PC_File_openHandle(pin);
    if (pout != INVALID_HANDLE_VALUE)
    {
        self->out = PC_File_openHandle(pout);
        PC_Event_register(PC_File_dataReadEvent(self->out), self, onStdoutData);
        PC_File_startReading(self->out);
    }
    if (perr != INVALID_HANDLE_VALUE)
    {
        self->err = PC_File_openHandle(perr);
        PC_Event_register(PC_File_dataReadEvent(self->err), self, onStderrData);
        PC_File_startReading(self->err);
    }

    PC_List *cmdline = PC_List_concat(self->executable, args);
    char *cmd = PC_List_joinStr(cmdline, " ");
    PC_List_destroy(cmdline);

    memset(&pi, 0, sizeof(pi));
    GetEnvironmentVariable("windir", windir, MAX_PATH);
    CreateProcess(0, cmd, 0, 0, 1, 0, 0, windir, &si, &pi);
    self->process = pi.hProcess;
    CloseHandle(pi.hThread);
    if (in != INVALID_HANDLE_VALUE && in != nul) CloseHandle(in);
    if (out != INVALID_HANDLE_VALUE && out != nul) CloseHandle(out);
    if (err != INVALID_HANDLE_VALUE && err != nul) CloseHandle(err);
    if (nul != INVALID_HANDLE_VALUE) CloseHandle(nul);

    return 1;
}

SOEXPORT int PC_ProcessHost_start(PC_ProcessHost *self, PC_ProcessHostFlags flags, ...)
{
    va_list ap;
    char *arg;
    PC_List *args = PC_List_create(0, 0, 0);

    va_start(ap, flags);
    while ((arg = va_arg(ap, char *)))
    {
        PC_List_append(args, arg);
    }
    va_end(ap);

    int rc = PC_ProcessHost_startv(self, flags, args);
    PC_List_destroy(args);
    return rc;
}

SOEXPORT PC_Event *PC_ProcessHost_stdoutDataEvent(const PC_ProcessHost *self)
{
    return self->stdoutData;
}

SOEXPORT PC_Event *PC_ProcessHost_stderrDataEvent(const PC_ProcessHost *self)
{
    return self->stderrData;
}

SOEXPORT void PC_ProcessHost_destroy(PC_ProcessHost *self)
{
    if (!self) return;
    PC_File_close(self->in);
    PC_File_close(self->out);
    PC_File_close(self->err);
    PC_Event_destroy(self->stdoutData);
    PC_Event_destroy(self->stderrData);
    if (self->process != INVALID_HANDLE_VALUE)
    {
        TerminateProcess(self->process, 0);
        WaitForSingleObject(self->process, INFINITE);
        CloseHandle(self->process);
    }
    PC_List_destroy(self->executable);
    free(self);
}
