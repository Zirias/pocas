#ifndef POCAS_CORE_PLUGIN_H
#define POCAS_CORE_PLUGIN_H

#include <pocas/core/decl.h>

C_CLASS_DECL(PC_List);

C_CLASS_DECL(PC_Plugin);

DECLEXPORT PC_Plugin *PC_Plugin_load(const char *path, const char *idSymbolName);
DECLEXPORT PC_List *PC_Plugin_loadDir(const char *path, const char *idSymbolName);
DECLEXPORT const char *PC_Plugin_path(const PC_Plugin *self);
DECLEXPORT const char *PC_Plugin_id(const PC_Plugin *self);
DECLEXPORT void *PC_Plugin_symbol(const PC_Plugin *self, const char *name);
DECLEXPORT void PC_Plugin_close(PC_Plugin *self);

#endif
