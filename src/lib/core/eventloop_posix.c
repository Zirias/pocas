#include "eventloop_internal.h"

static int posixEventProcessor(int timeout)
{
    return 0;
}

SOLOCAL PC_EventProcessor eventProcessor = &posixEventProcessor;
