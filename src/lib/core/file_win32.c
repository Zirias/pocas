#include <stdlib.h>
#include <string.h>

#include <pocas/core/list.h>
#include <pocas/core/event_win32.h>
#include <pocas/core/file_win32.h>

struct PC_File
{
    HANDLE handle;
    PC_Event *dataRead;
    OVERLAPPED ovwr;
};

SOEXPORT PC_File *PC_File_open(const char *path, PC_FileMode mode)
{
    if (!(mode & PC_FM_RW)) return 0;
    DWORD shareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
    DWORD creationDisposition = (mode & PC_FM_Create) ? OPEN_ALWAYS : OPEN_EXISTING;
    DWORD desiredAccess = 0;
    if (mode & PC_FM_Read) desiredAccess |= GENERIC_READ;
    if (mode & PC_FM_Write) desiredAccess |= GENERIC_WRITE;
    HANDLE handle = CreateFile(path, desiredAccess, shareMode, 0, creationDisposition, FILE_FLAG_OVERLAPPED, 0);
    if (handle == INVALID_HANDLE_VALUE) return 0;
    if (mode & PC_FM_Append)
    {
        LARGE_INTEGER distance;
        distance.QuadPart = 0;
        SetFilePointerEx(handle, distance, 0, FILE_END);
    }
    return PC_File_openHandle(handle);
}

SOEXPORT PC_File *PC_File_openHandle(HANDLE handle)
{
    PC_File *self = calloc(1, sizeof(PC_File));
    self->handle = handle;
    self->dataRead = PC_Event_create("Data");
    self->ovwr.hEvent = CreateEvent(0, 1, 0, 0);
    return self;
}

SOEXPORT void PC_File_startReading(PC_File *self)
{
    (void)self; // TODO
}

SOEXPORT void PC_File_stopReading(PC_File *self)
{
    (void)self; // TODO
}

SOEXPORT int PC_File_write(PC_File *self, void *buffer, size_t n, size_t *written)
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

SOEXPORT PC_Event *PC_File_dataReadEvent(const PC_File *self)
{
    return self->dataRead;
}

SOEXPORT void PC_File_close(PC_File *self)
{
    if (!self) return;
    PC_Event_destroy(self->dataRead);
    CloseHandle(self->handle);
    CloseHandle(self->ovwr.hEvent);
    free(self);
}

SOEXPORT PC_List *PC_File_findInDir(const char *path, const char *pattern)
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

    PC_List *found = PC_List_createStr(0);
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
        PC_List_append(found, f);
    } while (FindNextFile(findHdl, &findData));
    FindClose(findHdl);
    free(pat);
    return found;
}
