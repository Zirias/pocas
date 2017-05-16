#ifndef INTERNAL_H
#define INTERNAL_H

#include <pocas/gui/private/backend.h>

#define GUICLASSID "PcGC"

typedef struct GuiClass
{
    char id[4];
    void *backend;
    void *container;
    void *control;
} GuiClass;

#define GCINIT(gcobj) do { \
    GuiClass *gc = (GuiClass *)gcobj; \
    gc->id[0] = 'P'; \
    gc->id[1] = 'c'; \
    gc->id[2] = 'G'; \
    gc->id[3] = 'C'; \
    gc->backend = 0; \
    gc->container = 0; \
    gc->control = 0; \
    } while (0)

#define ISGCOBJ(gcobj) !strncmp(((const GuiClass*)gcobj)->id, GUICLASSID, 4)

extern const PG_PrivateApi privateApi;

#endif // INTERNAL_H
