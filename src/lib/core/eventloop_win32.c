#include "c11threads.h"

#include <string.h>
#include <windows.h>

#include <pocas/core/event_win32.h>
#include <pocas/core/eventloop_win32.h>

struct win32EventLoopData
{
    int initialized;
    PC_Win32HndlEvInfo hndlEvInfo;
    PC_Win32MsgEvInfo msgEvInfo;
    PC_Event *win32HndlEvent;
    PC_Event *win32MsgEvent;
    HANDLE *handles;
    DWORD maxHandles;
    DWORD numHandles;
    int processMessages;
    MSG msg;
};

static thread_local struct win32EventLoopData data = { 0 };

static void init(void)
{
    if (!data.initialized)
    {
        data.initialized = 1;
        data.win32HndlEvent = PC_Event_create("win32Hndl");
        data.win32MsgEvent = PC_Event_create("win32Msg");
        data.processMessages = 0;
    }
}

SOEXPORT PC_Event *PC_EventLoop_win32HndlEvent()
{
    init();
    return data.win32HndlEvent;
}

SOEXPORT PC_Event *PC_EventLoop_win32MsgEvent()
{
    init();
    return data.win32MsgEvent;
}

static int win32EventProcessor(int timeout)
{
    init();

    if (!data.numHandles && !data.processMessages)
    {
        PC_EventLoop_exit(EXIT_FAILURE);
        return 0;
    }

    DWORD rc;

    if (data.processMessages)
    {
        rc = MsgWaitForMultipleObjectsEx(data.numHandles,
                data.handles, (DWORD)timeout,
                QS_ALLEVENTS, MWMO_INPUTAVAILABLE);
    }
    else
    {
        rc = WaitForMultipleObjectsEx(data.numHandles,
                data.handles, 0, (DWORD)timeout, 1);
    }

    if (rc == WAIT_FAILED) return -1;
    if (rc == WAIT_TIMEOUT) return 0;
    if (data.processMessages && rc == WAIT_OBJECT_0 + data.numHandles)
    {
        int nevents = 0;
        while (PeekMessageW(&data.msg, 0, 0, 0, PM_REMOVE))
        {
            ++nevents;
            if (data.msg.message == WM_QUIT)
            {
                PC_EventLoop_exit((int)data.msg.wParam);
                return nevents;
            }
            PC_EventArgs args = PC_EventArgs_init(data.win32MsgEvent,
                    0, &data.msg);
            PC_Event_raise(data.win32MsgEvent, &args);
            if (!args.handled)
            {
                TranslateMessage(&data.msg);
                DispatchMessageW(&data.msg);
            }
        }
        return nevents;
    }
    else
    {
        int idx;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wtype-limits"
        if (rc >= WAIT_OBJECT_0 && rc < WAIT_OBJECT_0 + data.numHandles)
        {
            idx = rc - WAIT_OBJECT_0;
        }
        else if (rc >= WAIT_ABANDONED_0
                 && rc < WAIT_ABANDONED_0 + data.numHandles)
        {
            idx = rc - WAIT_ABANDONED_0;
        }
        else return -1;
#pragma GCC diagnostic pop

        data.hndlEvInfo.hndl = data.handles[idx];
        PC_EventArgs args = PC_EventArgs_init(data.win32HndlEvent,
                0, &data.hndlEvInfo);
        PC_Event_raise(data.win32HndlEvent, &args);
        return 1;
    }
}

SOEXPORT void PC_EventLoop_setProcessMessages(int processMessages)
{
    data.processMessages = !!processMessages;
}

SOLOCAL PC_EventProcessor eventProcessor = &win32EventProcessor;

/*
typedef struct WaitOvrdRecord
{
    HANDLE readHandle;
    const Event *event;
    OverlappedReadData *ovrd;
    void *sender;
} WaitOvrdRecord;

SOEXPORT int EventLoop_processEvents(int timeout)
{
    const List *eventLoopEntries = EventLoop_entries();
    ListIterator *i = List_iterator(eventLoopEntries);
    DWORD nhandles = 0;
    int processed = 0;
    while (ListIterator_moveNext(i))
    {
        EventLoopEntry *entry =  ListIterator_current(i);
        const PlatformEvent *pe = EventLoopEntry_platformEvent(entry);
        if (pe->type == PET_Handle)
        {
            if (pe->peData.petHandle.action == PEHA_ReadOverlapped)
            {
                OverlappedReadData *ovrd = pe->peData.petHandle.haData.ovRead;
                int isDiskFile = GetFileType(pe->peData.petHandle.handle) == FILE_TYPE_DISK;
                if (!ovrd->pending)
                {
                    if (isDiskFile)
                    {
                        LARGE_INTEGER dist, pos;
                        dist.QuadPart = 0;
                        if (SetFilePointerEx(pe->peData.petHandle.handle, dist, &pos, FILE_CURRENT))
                        {
                            ovrd->over.Offset = pos.LowPart;
                            ovrd->over.OffsetHigh = pos.HighPart;
                        }
                    }
                    DWORD n;
                    while (ReadFile(pe->peData.petHandle.handle,
                                 ovrd->buffer, ORD_BUFSIZE, &n, &ovrd->over))
                    {
                        if (isDiskFile)
                        {
                            LARGE_INTEGER dist, pos;
                            dist.QuadPart = n;
                            if (SetFilePointerEx(pe->peData.petHandle.handle, dist, &pos, FILE_CURRENT))
                            {
                                ovrd->over.Offset = pos.LowPart;
                                ovrd->over.OffsetHigh = pos.HighPart;
                            }
                        }
                        DataEventData *ed = DataEventData_create(
                                    ovrd->buffer, (size_t)n);
                        const Event *e = EventLoopEntry_event(entry);
                        EventArgs *args = EventArgs_create(
                                    e, EventLoopEntry_sender(entry), ed);
                        Event_raise(e, args);
                        DataEventData_destroy(ed);
                        EventArgs_destroy(args);
                        ++processed;
                        ovrd->eof = 0;
                    }
                    DWORD lastErr = GetLastError();
                    if (lastErr == ERROR_IO_PENDING)
                    {
                        ovrd->pending = 1;
                    }
                    else if (lastErr == ERROR_HANDLE_EOF || lastErr == ERROR_BROKEN_PIPE)
                    {
                        if (!ovrd->eof)
                        {
                            DataEventData *ed = DataEventData_create(0, 0);
                            const Event *e = EventLoopEntry_event(entry);
                            EventArgs *args = EventArgs_create(
                                        e, EventLoopEntry_sender(entry), ed);
                            Event_raise(e, args);
                            DataEventData_destroy(ed);
                            EventArgs_destroy(args);
                            ++processed;
                            ovrd->eof = 1;
                        }
                    }
                }
                if (ovrd->pending)
                {
                    ++nhandles;
                }
            }
        }
    }

    WaitOvrdRecord *waitOvrdRecords = malloc(nhandles * sizeof(WaitOvrdRecord));
    HANDLE *waitOvrdHandles = malloc(nhandles * sizeof(HANDLE));
    DWORD idx = 0;
    while (ListIterator_moveNext(i))
    {
        EventLoopEntry *entry =  ListIterator_current(i);
        const PlatformEvent *pe = EventLoopEntry_platformEvent(entry);
        if (pe->type == PET_Handle)
        {
            if (pe->peData.petHandle.action == PEHA_ReadOverlapped)
            {
                OverlappedReadData *ovrd = pe->peData.petHandle.haData.ovRead;
                if (ovrd->pending)
                {
                    waitOvrdRecords[idx].event = EventLoopEntry_event(entry);
                    waitOvrdRecords[idx].readHandle = pe->peData.petHandle.handle;
                    waitOvrdRecords[idx].sender = EventLoopEntry_sender(entry);
                    waitOvrdRecords[idx].ovrd = ovrd;
                    waitOvrdHandles[idx++] = ovrd->over.hEvent;
                }
            }
        }
    }
    ListIterator_destroy(i);

    DWORD rc = WaitForMultipleObjects(nhandles, waitOvrdHandles, FALSE, timeout);
    if (rc != WAIT_FAILED && rc != WAIT_TIMEOUT) do
    {
        idx = rc - WAIT_OBJECT_0;
        DWORD n;
        int success = GetOverlappedResult(waitOvrdRecords[idx].readHandle,
                            &waitOvrdRecords[idx].ovrd->over, &n, FALSE);
        if (success)
        {
            if (n && GetFileType(waitOvrdRecords[idx].readHandle) == FILE_TYPE_DISK)
            {
                LARGE_INTEGER dist;
                dist.QuadPart = n;
                SetFilePointerEx(waitOvrdRecords[idx].readHandle, dist, 0, FILE_CURRENT);
            }
            DataEventData *ed = DataEventData_create(
                        waitOvrdRecords[idx].ovrd->buffer, (size_t)n);
            const Event *e = waitOvrdRecords[idx].event;
            EventArgs *args = EventArgs_create(
                        e, waitOvrdRecords[idx].sender, ed);
            Event_raise(e, args);
            DataEventData_destroy(ed);
            EventArgs_destroy(args);
            ++processed;
            waitOvrdRecords[idx].ovrd->eof = 0;
        }
        DWORD lastErr = GetLastError();
        if (!success &&
                (lastErr == ERROR_HANDLE_EOF || lastErr == ERROR_BROKEN_PIPE))
        {
            if (!waitOvrdRecords[idx].ovrd->eof)
            {
                DataEventData *ed = DataEventData_create(0, 0);
                const Event *e = waitOvrdRecords[idx].event;
                EventArgs *args = EventArgs_create(
                            e, waitOvrdRecords[idx].sender, ed);
                Event_raise(e, args);
                DataEventData_destroy(ed);
                EventArgs_destroy(args);
                ++processed;
                waitOvrdRecords[idx].ovrd->eof = 1;
            }
        }
        waitOvrdRecords[idx].ovrd->pending = 0;
        ResetEvent(waitOvrdHandles[idx]);
        rc = WaitForMultipleObjects(nhandles, waitOvrdHandles, FALSE, 0);
    } while (rc != WAIT_FAILED && rc != WAIT_TIMEOUT);

    free(waitOvrdHandles);
    free(waitOvrdRecords);

    return processed;
}
*/
