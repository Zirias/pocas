#include <stdlib.h>
#include <stdio.h>

#include <pocas/core/event.h>
#include <pocas/core/eventloop.h>

#include <pocas/gui/bounds.h>
#include <pocas/gui/command.h>
#include <pocas/gui/container.h>
#include <pocas/gui/control.h>
#include <pocas/gui/extents.h>
#include <pocas/gui/hbox.h>
#include <pocas/gui/label.h>
#include <pocas/gui/menu.h>
#include <pocas/gui/messagebox.h>
#include <pocas/gui/window.h>

#include <pocas/test/decl.h>

#include "gui_internal.h"

struct Gui
{
    int disposed;
    Window *mainWindow;
    Menu *mainMenu;
    Command *closeCommand;
    Label *middleLabel;
};

static void handleCloseCommand(void *selfPtr, EventArgs *args)
{
    Gui *self = selfPtr;
    Window_close(self->mainWindow);
    EventArgs_setHandled(args);
}

static void handleWindowClosing(void *selfPtr, EventArgs *args)
{
    Gui *self = selfPtr;

    MessageBoxButton result = MessageBox_show(self->mainWindow, "Really quit?",
            "Are you sure you want to quit?", MBB_Yes|MBB_No, MBS_Warning);

    if (result == MBB_No)
    {
        EventArgs_setHandled(args);
    }
}

static void handleContainerResized(void *selfPtr, EventArgs *args)
{
    char labeltext[20];
    Gui *self = selfPtr;

    Bounds *b = EventArgs_evInfo(args);
    snprintf(labeltext, 20, "(2) %u x %u", b->width, b->height);
    Label_setText(self->middleLabel, labeltext);
}

SOLOCAL Gui *Gui_create(void)
{
    Gui *self = malloc(sizeof(Gui));
    self->disposed = 0;

    self->closeCommand = Command_create();
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

    HBox *hb = HBox_create();

    Extents margin = {8,8,8,8};

    Label *lbl = Label_create("(1) This is a test!");
    Control_show(lbl);
    Control_setMargin(lbl, &margin);
    HBox_addControl(hb, lbl);
    self->middleLabel = Label_create("(2) ♫ 42");
    Control_show(self->middleLabel);
    HBox_addControl(hb, self->middleLabel);
    lbl = Label_create("(3) This is a test!");
    Control_show(lbl);
    Control_setMargin(lbl, &margin);
    HBox_addControl(hb, lbl);

    margin.top = 24;
    Control_setMargin(self->middleLabel, &margin);
    Control_setMargin(hb, &margin);

    Container_setControl(self->mainWindow, hb);

    Event_register(Command_invokedEvent(self->closeCommand),
            self, handleCloseCommand);

    Event_register(Window_closingEvent(self->mainWindow),
            self, handleWindowClosing);

    Event_register(Container_resizedEvent(self->mainWindow),
            self, handleContainerResized);

    return self;
}

SOLOCAL int Gui_run(Gui *self)
{
    Control_show(self->mainWindow);
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
