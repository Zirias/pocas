#ifndef POCAS_CORE_EVENT_POSIX_H
#define POCAS_CORE_EVENT_POSIX_H

#include <pocas/core/event.h>

enum PC_PlatformEventType
{
    PC_PET_Fd
};
C_ENUM_DECL(PC_PlatformEventType);

enum PC_PlatformEventHandleAction
{
    PC_PEFD_SelectRead
};
C_ENUM_DECL(PC_PlatformEventHandleAction);


#endif
