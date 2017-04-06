#ifndef POCAS_CORE_PLUGIN_H
#define POCAS_CORE_PLUGIN_H

#include <pocas/core/decl.h>

typedef struct List List;

typedef struct Plugin Plugin;

DECLEXPORT Plugin *Plugin_load(const char *path, const char *idSymbolName);
DECLEXPORT List *Plugin_loadDir(const char *path, const char *idSymbolName);
DECLEXPORT const char *Plugin_path(const Plugin *self);
DECLEXPORT const char *Plugin_id(const Plugin *self);
DECLEXPORT void *Plugin_symbol(const Plugin *self, const char *name);
DECLEXPORT void Plugin_close(Plugin *self);

#endif
