#include "bqt_control.h"
#include "bqt_backend.h"

#include <pocas/gui/backend/qt.h>

SOLOCAL Bqt_Control::Bqt_Control()
{
    Bqt_Backend_init();
}

SOLOCAL_CDECL void Bqt_Control_setShown(void *control, int shown)
{
    Bqt_Control *bc = (Bqt_Control *)
            PG_qtBackend->privateApi->backendObject(control);
    if (bc) bc->setShown(shown);
}
