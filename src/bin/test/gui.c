#include <pocas/core/event.h>
#include <pocas/core/eventloop.h>

#include <pocas/gui/command.h>
#include <pocas/gui/menu.h>
#include <pocas/gui/window.h>

#include <pocas/test/decl.h>

static Window *mainWindow;

void handleCloseCommand(void *self, EventArgs *args)
{
    (void)self;
    Window_destroy(mainWindow);
    EventArgs_setHandled(args);
}

SOLOCAL int guiMain(void)
{
    Command *closeCommand = Command_create();
    Event_register(Command_invokedEvent(closeCommand), 0, handleCloseCommand);

    Menu *mainMenu = Menu_create();
    Menu *fileMenu = Menu_create();

    MenuItem *item = MenuItem_create("E&xit");
    MenuItem_setCommand(item, closeCommand);
    Menu_addItem(fileMenu, item);

    item = MenuItem_create("&File");
    MenuItem_setSubMenu(item, fileMenu);
    Menu_addItem(mainMenu, item);

    mainWindow = Window_create("POCAS Test", 800, 600);
    Window_setMenu(mainWindow, mainMenu);
    Window_show(mainWindow);

    int rc = EventLoop_run();

    Menu_destroy(mainMenu);
    return rc;
}
