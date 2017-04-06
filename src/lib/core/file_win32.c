#include <stdlib.h>
#include <string.h>

#include <pocas/core/list.h>
#include <pocas/core/event_win32.h>
#include <pocas/core/file_win32.h>

struct File
{
    HANDLE handle;
    Event *dataRead;
    PlatformEvent *peDataRead;
    OVERLAPPED ovwr;
};

SOEXPORT File *File_open(const char *path, FileMode mode)
{
    if (!(mode & FM_RW)) return 0;
    DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    DWORD creationDisposition = (mode & FM_Create) ? OPEN_ALWAYS : OPEN_EXISTING;
    DWORD desiredAccess = 0;
    if (mode & FM_Read) desiredAccess |= GENERIC_READ;
    if (mode & FM_Write) desiredAccess |= GENERIC_WRITE;
    HANDLE handle = CreateFile(path, desiredAccess, shareMode, 0, creationDisposition, FILE_FLAG_OVERLAPPED, 0);
    if (handle == INVALID_HANDLE_VALUE) return 0;
    if (mode & FM_Append)
    {
        LARGE_INTEGER distance;
        distance.QuadPart = 0;
        SetFilePointerEx(handle, distance, 0, FILE_END);
    }
    return File_openHandle(handle);
}

SOEXPORT File *File_openHandle(HANDLE handle)
{
    File *self = calloc(1, sizeof(File));
    self->handle = handle;
    self->dataRead = Event_create("Data");
    self->ovwr.hEvent = CreateEvent(0, 1, 0, 0);
    return self;
}

SOEXPORT void File_startReading(File *self)
{
    if (self->peDataRead) return;
    self->peDataRead = PlatformEvent_fromHandle(self->handle, PEHA_ReadOverlapped);
    Event_registerWithLoop(self->dataRead, self->peDataRead, self);
}

SOEXPORT void File_stopReading(File *self)
{
    if (!self->peDataRead) return;
    Event_unregisterWithLoop(self->dataRead);
    PlatformEvent_destroy(self->peDataRead);
    self->peDataRead = 0;
}

SOEXPORT int File_write(File *self, void *buffer, size_t n, size_t *written)
{
    if (GetFileType(self->handle) == FILE_TYPE_DISK)
    {
        LARGE_INTEGER dist, pos;
        dist.QuadPart = 0;
        SetFilePointerEx(self->handle, dist, &pos, FILE_CURRENT);
        self->ovwr.Offset = pos.LowPart;
        self->ovwr.OffsetHigh = pos.HighPart;
    }

    DWORD w;
    if (!WriteFile(self->handle, buffer, (DWORD)n, &w, &self->ovwr))
    {
        if (GetLastError() != ERROR_IO_PENDING)
        {
            return 0;
        }
        if (!WaitForSingleObject(self->ovwr.hEvent, INFINITE))
        {
            return 0;
        }
        if (!GetOverlappedResult(self->handle, &self->ovwr, &w, 0))
        {
            return 0;
        }
    }

    if (GetFileType(self->handle) == FILE_TYPE_DISK)
    {
        LARGE_INTEGER dist;
        dist.QuadPart = w;
        SetFilePointerEx(self->handle, dist, 0, FILE_CURRENT);
    }

    if (written) *written = (size_t)w;

    return 1;
}

SOEXPORT EVENT(File, dataRead)
{
    return self->dataRead;
}

SOEXPORT void File_close(File *self)
{
    if (!self) return;
    if (self->peDataRead) File_stopReading(self);
    Event_destroy(self->dataRead);
    CloseHandle(self->handle);
    CloseHandle(self->ovwr.hEvent);
    free(self);
}

SOEXPORT List *File_findInDir(const char *path, const char *pattern)
{
    size_t pathlen = strlen(path);
    char *pat;
    if (path[pathlen - 1] == '\\')
    {
        pat = malloc(pathlen + strlen(pattern) + 1);
        strcpy(pat, path);
    }
    else
    {
        pat = malloc(pathlen + strlen(pattern) + 2);
        strcpy(pat, path);
        strcat(pat, "\\");
    }
    strcat(pat, pattern);

    WIN32_FIND_DATA findData;
    HANDLE findHdl = FindFirstFile(pat, &findData);
    if (findHdl == INVALID_HANDLE_VALUE)
    {
        free(pat);
        return 0;
    }

    List *found = List_createStr(0);
    do
    {
        char *f;
        if (path[pathlen - 1] == '\\')
        {
            f = malloc(pathlen + strlen(findData.cFileName) + 1);
            strcpy(f, path);
        }
        else
        {
            f = malloc(pathlen + strlen(findData.cFileName) + 2);
            strcpy(f, path);
            strcat(f, "\\");
        }
        strcat(f, findData.cFileName);
        List_append(found, f);
    } while (FindNextFile(findHdl, &findData));
    FindClose(findHdl);
    free(pat);
    return found;
}
