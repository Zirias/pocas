#include <stdlib.h>
#include <stdio.h>

#include <pocas/core/event.h>
#include <pocas/core/eventloop.h>

#include <pocas/gui/bounds.h>
#include <pocas/gui/button.h>
#include <pocas/gui/command.h>
#include <pocas/gui/container.h>
#include <pocas/gui/control.h>
#include <pocas/gui/extents.h>
#include <pocas/gui/lbox.h>
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
    Label *boundsLabel;
};

static void handleCloseCommand(void *selfPtr, EventArgs *args)
{
    Gui *self = selfPtr;
    Window_close(self->mainWindow);
    EventArgs_setHandled(args);
}

static void handleButtonClicked(void *selfPtr, EventArgs *args)
{
    Gui *self = selfPtr;

    MessageBoxButton result = MessageBox_show(self->mainWindow,
            "Button clicked", "Cancel this event?",
            MBB_Yes|MBB_No, MBS_Question);

    if (result == MBB_Yes)
    {
        EventArgs_setHandled(args);
    }
}

static void handleMenuItemSelected(void *selfPtr, EventArgs *args)
{
    Gui *self = selfPtr;

    MessageBoxButton result = MessageBox_show(self->mainWindow,
            "Menu item selected", "Cancel this event?",
            MBB_Yes|MBB_No, MBS_Question);

    if (result == MBB_Yes)
    {
        EventArgs_setHandled(args);
    }
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

static void showSize(Gui *self, Bounds *b)
{
    char labeltext[20];
    snprintf(labeltext, 20, "[ %u x %u ]", b->width, b->height);
    Label_setText(self->boundsLabel, labeltext);
}

static void handleContainerResized(void *selfPtr, EventArgs *args)
{
    Gui *self = selfPtr;
    Bounds *b = EventArgs_evInfo(args);
    showSize(self, b);
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
    Event_register(MenuItem_selectedEvent(item),
            self, handleMenuItemSelected);

    item = MenuItem_create("&File");
    MenuItem_setSubMenu(item, fileMenu);
    Menu_addItem(self->mainMenu, item);

    self->mainWindow = Window_create("POCAS Test", 320, 200);
    Window_setMenu(self->mainWindow, self->mainMenu);

    LBox *vb = LBox_create(BO_Vertical);
    LBox *hb = LBox_create(BO_Horizontal);

    Extents margin = {8,8,8,8};

    Label *lbl = Label_create("(1) This is a test!");
    Control_show(lbl);
    Control_setMargin(lbl, &margin);
    LBox_addControl(hb, lbl);
    Button *btn = Button_create("(2) ♫ 42");
    Button_setCommand(btn, self->closeCommand);
    Control_show(btn);
    Control_setMargin(btn, &margin);
    Event_register(Button_clickedEvent(btn),
            self, handleButtonClicked);
    LBox_addControl(hb, btn);
    lbl = Label_create("(3) This is a test!");
    Control_show(lbl);
    Control_setMargin(lbl, &margin);
    LBox_addControl(hb, lbl);

    margin.bottom = 24;
    self->boundsLabel = Label_create(0);
    Control_show(self->boundsLabel);
    Control_setMargin(self->boundsLabel, &margin);
    LBox_addControl(vb, self->boundsLabel);
    LBox_addControl(vb, hb);

    Container_setControl(self->mainWindow, vb);

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
    Bounds b;
    Container_bounds(self->mainWindow, &b);
    showSize(self, &b);
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
