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

struct File
{
    int fd;
    Event *dataRead;
};

SOEXPORT File *File_open(const char *path, FileMode mode)
{
    int flags = 0;
    if ((mode & FM_RW) == FM_RW) flags |= O_RDWR;
    else if (mode & FM_Read) flags |= O_RDONLY;
    else if (mode & FM_Write) flags |= O_WRONLY;
    if (mode & FM_Create) flags |= O_CREAT;
    if (mode & FM_Append) flags |= O_APPEND;
    int fd = open(path, flags, S_IRWXU);
    if (fd < 0) return 0;
    return File_openFd(fd);
}

SOEXPORT File *File_openFd(int fd)
{
    File *self = malloc(sizeof(File));
    self->fd = fd;
    self->dataRead = Event_create("Data");
    return self;
}

SOEXPORT List *File_findInDir(const char *path, const char *pattern)
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

    List *found = List_createStr(0);
    for (char **pathvp = glb.gl_pathv; pathvp && *pathvp; ++pathvp)
    {
	List_append(found, String_copy(*pathvp));
    }

    globfree(&glb);
    return found;
}

