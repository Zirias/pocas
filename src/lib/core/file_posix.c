#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <pocas/core/event_posix.h>
#include <pocas/core/file_posix.h>

struct File
{
    int fd;
    Event *dataRead;
    PlatformEvent *peDataRead;
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
    self->peDataRead = 0;
    return self;
}

