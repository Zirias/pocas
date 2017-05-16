#include <stdlib.h>

#include <pocas/core/dso.h>
#include <pocas/core/file.h>
#include <pocas/core/list.h>
#include <pocas/core/plugin.h>
#include <pocas/core/string.h>

struct PC_Plugin
{
    const char *id;
    char *path;
    PC_Dso *dso;
};

extern const char *plugin_pattern;

SOEXPORT PC_Plugin *PC_Plugin_load(const char *path, const char *idSymbolName)
{
    PC_Dso *so = PC_Dso_load(path);
    if (!so) return 0;
    const char *id = PC_Dso_string(so, idSymbolName);
    if (!id)
    {
        PC_Dso_close(so);
        return 0;
    }

    PC_Plugin *self = malloc(sizeof(PC_Plugin));
    self->path = PC_String_copy(path);
    self->id = id;
    self->dso = so;

    return self;
}

SOEXPORT PC_List *PC_Plugin_loadDir(const char *path, const char *idSymbolName)
{
    PC_List *pluginFiles = PC_File_findInDir(path, plugin_pattern);
    if (!pluginFiles) return 0;
    PC_List *plugins = PC_List_create(0, 0, 0);
    PC_ListIterator *i = PC_List_iterator(pluginFiles);
    while (PC_ListIterator_moveNext(i))
    {
        PC_Plugin *p = PC_Plugin_load(PC_ListIterator_current(i), idSymbolName);
        if (p) PC_List_append(plugins, p);
    }
    PC_ListIterator_destroy(i);
    PC_List_destroy(pluginFiles);
    if (!PC_List_length(plugins))
    {
        PC_List_destroy(plugins);
        return 0;
    }
    return plugins;
}

SOEXPORT const char *PC_Plugin_id(const PC_Plugin *self)
{
    return self->id;
}

SOEXPORT const char *PC_Plugin_path(const PC_Plugin *self)
{
    return self->path;
}

SOEXPORT void *PC_Plugin_symbol(const PC_Plugin *self, const char *name)
{
    return PC_Dso_symbol(self->dso, name);
}

SOEXPORT void PC_Plugin_close(PC_Plugin *self)
{
    if (!self) return;
    PC_Dso_close(self->dso);
    free(self->path);
    free(self);
}
