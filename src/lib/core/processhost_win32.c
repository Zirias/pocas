#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>

#include <pocas/core/event.h>
#include <pocas/core/string.h>
#include <pocas/core/list.h>
#include <pocas/core/file_win32.h>

#include <pocas/core/processhost.h>

struct ProcessHost
{
    List *executable;
    HANDLE process;
    File *in;
    File *out;
    File *err;
    Event *stdoutData;
    Event *stderrData;
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

static void onStdoutData(void *selfptr, EventArgs *args)
{
    ProcessHost *self = selfptr;
    EventArgs stdoutArgs = EventArgs_init(
                self->stdoutData, self, args->evInfo);
    Event_raise(self->stdoutData, &stdoutArgs);
}

static void onStderrData(void *selfptr, EventArgs *args)
{
    ProcessHost *self = selfptr;
    EventArgs stderrArgs = EventArgs_init(
                self->stderrData, self, args->evInfo);
    Event_raise(self->stderrData, &stderrArgs);
}

SOEXPORT ProcessHost *ProcessHost_create(const char *executable)
{
    ProcessHost *self = malloc(sizeof(ProcessHost));
    self->executable = List_createStr(1);
    List_append(self->executable, String_copy(executable));
    self->process = INVALID_HANDLE_VALUE;
    self->in = 0;
    self->out = 0;
    self->err = 0;
    self->stdoutData = Event_create("Data");
    self->stderrData = Event_create("Data");
    return self;
}

SOEXPORT int ProcessHost_startv(ProcessHost *self, ProcessHostFlags flags, const List *args)
{
    char windir[MAX_PATH];
    SECURITY_ATTRIBUTES sa;
    PROCESS_INFORMATION pi;
    STARTUPINFO si;
    HANDLE nul, in, out, err, pin, pout, perr;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = 1;
    sa.lpSecurityDescriptor = 0;

    if (flags & PH_NO_STDIO)
    {
        nul = CreateFile("nul", GENERIC_READ|GENERIC_WRITE, 0, &sa, 0, 0, 0);
    }
    else
    {
        nul = INVALID_HANDLE_VALUE;
    }
    in = flags & PH_NO_STDIN ? nul : INVALID_HANDLE_VALUE;
    pin = INVALID_HANDLE_VALUE;
    out = flags & PH_NO_STDOUT ? nul : INVALID_HANDLE_VALUE;
    pout = INVALID_HANDLE_VALUE;
    err = flags & PH_NO_STDERR ? nul : INVALID_HANDLE_VALUE;
    perr = INVALID_HANDLE_VALUE;

    if (flags & PH_REDIR_STDIN && in == INVALID_HANDLE_VALUE)
    {
        mkpipe(&in, &pin, &sa, 0, 0, 0);
    }

    if (flags & PH_REDIR_STDOUT && out == INVALID_HANDLE_VALUE)
    {
        mkpipe(&pout, &out, 0, &sa, FILE_FLAG_OVERLAPPED, 0);
    }

    if (flags & PH_REDIR_STDERR && !(flags & PH_REDIR_STDERR_IS_STDOUT)
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
                flags & PH_REDIR_STDERR_IS_STDOUT ?
                    si.hStdOutput :
                    GetStdHandle(STD_ERROR_HANDLE)
                  : err;
    si.dwFlags |= STARTF_USESTDHANDLES;

    if (pin != INVALID_HANDLE_VALUE) self->in = File_openHandle(pin);
    if (pout != INVALID_HANDLE_VALUE)
    {
        self->out = File_openHandle(pout);
        Event_register(File_dataReadEvent(self->out), self, onStdoutData);
        File_startReading(self->out);
    }
    if (perr != INVALID_HANDLE_VALUE)
    {
        self->err = File_openHandle(perr);
        Event_register(File_dataReadEvent(self->err), self, onStderrData);
        File_startReading(self->err);
    }

    List *cmdline = List_concat(self->executable, args);
    char *cmd = List_joinStr(cmdline, " ");
    List_destroy(cmdline);

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

SOEXPORT int ProcessHost_start(ProcessHost *self, ProcessHostFlags flags, ...)
{
    va_list ap;
    char *arg;
    List *args = List_create(0, 0, 0);

    va_start(ap, flags);
    while ((arg = va_arg(ap, char *)))
    {
        List_append(args, arg);
    }
    va_end(ap);

    int rc = ProcessHost_startv(self, flags, args);
    List_destroy(args);
    return rc;
}

SOEXPORT Event *ProcessHost_stdoutDataEvent(const ProcessHost *self)
{
    return self->stdoutData;
}

SOEXPORT Event *ProcessHost_stderrDataEvent(const ProcessHost *self)
{
    return self->stderrData;
}

SOEXPORT void ProcessHost_destroy(ProcessHost *self)
{
    if (!self) return;
    File_close(self->in);
    File_close(self->out);
    File_close(self->err);
    Event_destroy(self->stdoutData);
    Event_destroy(self->stderrData);
    if (self->process != INVALID_HANDLE_VALUE)
    {
        TerminateProcess(self->process, 0);
        WaitForSingleObject(self->process, INFINITE);
        CloseHandle(self->process);
    }
    List_destroy(self->executable);
    free(self);
}
