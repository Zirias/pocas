#include <pocas/core/eventloop.h>
#include <pocas/gui/window.h>

#include <pocas/test/decl.h>

#include "gui_internal.h"

SOLOCAL int guiMain(void)
{
    Window *mainWindow = Window_create("POCAS Test", 800, 600);
    Window_show(mainWindow);

    int rc = EventLoop_run();

    Window_destroy(mainWindow);
    return rc;
}
