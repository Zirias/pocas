#include <stdlib.h>

#include <pocas/core/dso.h>
#include <pocas/core/file.h>
#include <pocas/core/list.h>
#include <pocas/core/plugin.h>
#include <pocas/core/string.h>

struct Plugin
{
    const char *id;
    char *path;
    Dso *dso;
};

extern const char *plugin_pattern;

SOEXPORT Plugin *Plugin_load(const char *path, const char *idSymbolName)
{
    Dso *so = Dso_load(path);
    if (!so) return 0;
    const char *id = Dso_string(so, idSymbolName);
    if (!id)
    {
        Dso_close(so);
        return 0;
    }

    Plugin *self = malloc(sizeof(Plugin));
    self->path = String_copy(path);
    self->id = id;
    self->dso = so;

    return self;
}

SOEXPORT List *Plugin_loadDir(const char *path, const char *idSymbolName)
{
    List *pluginFiles = File_findInDir(path, plugin_pattern);
    if (!pluginFiles) return 0;
    List *plugins = List_create(0, 0, 0);
    ListIterator *i = List_iterator(pluginFiles);
    while (ListIterator_moveNext(i))
    {
        Plugin *p = Plugin_load(ListIterator_current(i), idSymbolName);
        if (p) List_append(plugins, p);
    }
    ListIterator_destroy(i);
    List_destroy(pluginFiles);
    if (!List_length(plugins))
    {
        List_destroy(plugins);
        return 0;
    }
    return plugins;
}

SOEXPORT const char *Plugin_id(const Plugin *self)
{
    return self->id;
}

SOEXPORT const char *Plugin_path(const Plugin *self)
{
    return self->path;
}

SOEXPORT void *Plugin_symbol(const Plugin *self, const char *name)
{
    return Dso_symbol(self->dso, name);
}

SOEXPORT void Plugin_close(Plugin *self)
{
    if (!self) return;
    Dso_close(self->dso);
    free(self->path);
    free(self);
}
