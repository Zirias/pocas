#ifndef POCAS_CORE_FILE_H
#define POCAS_CORE_FILE_H

#include <stddef.h>

#include <pocas/core/event.h>

typedef struct List List;

typedef struct File File;

typedef enum FileMode
{
    FM_Read = 1 << 0,
    FM_Write = 1 << 1,
    FM_RW = FM_Read | FM_Write,
    FM_Append = 1 << 2,
    FM_Create = 1 << 3
} FileMode;

DECLEXPORT File *File_open(const char *path, FileMode mode);
DECLEXPORT void File_startReading(File *self);
DECLEXPORT void File_stopReading(File *self);
DECLEXPORT int File_write(File *self, void *buffer, size_t n, size_t *written);
DECLEXPORT void File_close(File *self);

DECLEXPORT EVENT(File, dataRead);

DECLEXPORT List *File_findInDir(const char *path, const char *pattern);

#endif
