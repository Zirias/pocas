#ifndef POCAS_CORE_FILE_H
#define POCAS_CORE_FILE_H

#include <stddef.h>

#include <pocas/core/decl.h>

C_CLASS_DECL(PC_Event);
C_CLASS_DECL(PC_List);

C_CLASS_DECL(PC_File);

enum PC_FileMode
{
    PC_FM_Read = 1 << 0,
    PC_FM_Write = 1 << 1,
    PC_FM_RW = PC_FM_Read | PC_FM_Write,
    PC_FM_Append = 1 << 2,
    PC_FM_Create = 1 << 3
};
C_ENUM_DECL(PC_FileMode);

DECLEXPORT PC_File *PC_File_open(const char *path, PC_FileMode mode);
DECLEXPORT void PC_File_startReading(PC_File *self);
DECLEXPORT void PC_File_stopReading(PC_File *self);
DECLEXPORT int PC_File_write(PC_File *self, void *buffer, size_t n, size_t *written);
DECLEXPORT void PC_File_close(PC_File *self);

DECLEXPORT PC_Event *PC_File_dataReadEvent(const PC_File *self);

DECLEXPORT PC_List *PC_File_findInDir(const char *path, const char *pattern);

#endif
