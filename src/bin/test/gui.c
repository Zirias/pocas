#include <stdlib.h>

#include <pocas/core/event.h>
#include <pocas/core/eventloop.h>

#include <pocas/gui/command.h>
#include <pocas/gui/menu.h>
#include <pocas/gui/window.h>

#include <pocas/test/decl.h>

#include "gui_internal.h"

struct Gui
{
    int disposed;
    Window *mainWindow;
    Menu *mainMenu;
    Command *closeCommand;
};

void handleCloseCommand(void *selfPtr, EventArgs *args)
{
    Gui *self = selfPtr;
    Gui_dispose(self);
    EventArgs_setHandled(args);
}

SOLOCAL Gui *Gui_create(void)
{
    Gui *self = malloc(sizeof(Gui));
    self->disposed = 0;

    self->closeCommand = Command_create();
    Event_register(Command_invokedEvent(self->closeCommand),
            self, handleCloseCommand);

    self->mainMenu = Menu_create();
    Menu *fileMenu = Menu_create();

    MenuItem *item = MenuItem_create("E&xit");
    MenuItem_setCommand(item, self->closeCommand);
    Menu_addItem(fileMenu, item);

    item = MenuItem_create("&File");
    MenuItem_setSubMenu(item, fileMenu);
    Menu_addItem(self->mainMenu, item);

    self->mainWindow = Window_create("POCAS Test", 800, 600);
    Window_setMenu(self->mainWindow, self->mainMenu);

    return self;
}

SOLOCAL int Gui_run(Gui *self)
{
    Window_show(self->mainWindow);
    return EventLoop_run();
}

SOLOCAL void Gui_dispose(Gui *self)
{
    if (self->disposed) return;
    Window_destroy(self->mainWindow);
    Menu_destroy(self->mainMenu);
    Command_destroy(self->closeCommand);
    self->disposed = 1;
}

SOLOCAL void Gui_destroy(Gui *self)
{
    if (!self) return;
    Gui_dispose(self);
    free(self);
}
