#ifndef POCAS_CORE_EVENT_H
#define POCAS_CORE_EVENT_H

#include <stddef.h>

#include <pocas/core/decl.h>

C_CLASS_DECL(PC_Event);

C_CLASS_DECL(PC_EventArgs);
struct PC_EventArgs
{
    PC_Event *const event;
    void *const sender;
    void *const evInfo;
    int handled;
};

C_CLASS_DECL(PC_DataEvInfo);
struct PC_DataEvInfo
{
    size_t size;
    char data[];
};

typedef void (*PC_EventHandler)(void *self, PC_EventArgs *args);

DECLEXPORT PC_Event *PC_Event_create(const char *name);
DECLEXPORT void PC_Event_register(PC_Event *self, void *object, PC_EventHandler handler);
DECLEXPORT void PC_Event_unregister(PC_Event *self, void *object, PC_EventHandler handler);
DECLEXPORT int PC_Event_count(const PC_Event *self);
DECLEXPORT void PC_Event_clear(PC_Event *self);
DECLEXPORT void PC_Event_raise(const PC_Event *self, PC_EventArgs *args);
DECLEXPORT const char *PC_Event_name(const PC_Event *self);
DECLEXPORT void PC_Event_destroy(PC_Event *self);

#define PC_EventArgs_init(event, sender, evInfo) {(event), (sender), (evInfo), 0}

#endif
