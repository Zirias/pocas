#ifndef POCAS_CORE_FILE_WIN32_H
#define POCAS_CORE_FILE_WIN32_H

#include <windows.h>

#include <pocas/core/file.h>

DECLEXPORT File *File_openHandle(HANDLE handle);

#endif
