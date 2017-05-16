#include <fcntl.h>
#include <glob.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <pocas/core/event_posix.h>
#include <pocas/core/file_posix.h>
#include <pocas/core/list.h>
#include <pocas/core/string.h>

struct PC_File
{
    int fd;
    PC_Event *dataRead;
};

SOEXPORT PC_File *PC_File_open(const char *path, PC_FileMode mode)
{
    int flags = 0;
    if ((mode & PC_FM_RW) == PC_FM_RW) flags |= O_RDWR;
    else if (mode & PC_FM_Read) flags |= O_RDONLY;
    else if (mode & PC_FM_Write) flags |= O_WRONLY;
    if (mode & PC_FM_Create) flags |= O_CREAT;
    if (mode & PC_FM_Append) flags |= O_APPEND;
    int fd = open(path, flags, S_IRWXU);
    if (fd < 0) return 0;
    return PC_File_openFd(fd);
}

SOEXPORT PC_File *PC_File_openFd(int fd)
{
    PC_File *self = malloc(sizeof(PC_File));
    self->fd = fd;
    self->dataRead = PC_Event_create("Data");
    return self;
}

SOEXPORT PC_List *PC_File_findInDir(const char *path, const char *pattern)
{
    size_t pathlen = strlen(path);
    char *pat;
    if (path[pathlen - 1] == '/')
    {
        pat = malloc(pathlen + strlen(pattern) + 1);
        strcpy(pat, path);
    }
    else
    {
        pat = malloc(pathlen + strlen(pattern) + 2);
        strcpy(pat, path);
        strcat(pat, "/");
    }
    strcat(pat, pattern);

    glob_t glb;
    glob(pat, 0, 0, &glb);

    free(pat);

    PC_List *found = PC_List_createStr(0);
    for (char **pathvp = glb.gl_pathv; pathvp && *pathvp; ++pathvp)
    {
        PC_List_append(found, PC_String_copy(*pathvp));
    }

    globfree(&glb);
    return found;
}

