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
#include <pocas/gui/textbox.h>

#include <pocas/test/decl.h>

#include "gui_internal.h"

struct Gui
{
    int disposed;
    Window *mainWindow;
    Menu *mainMenu;
    Command *closeCommand;
    LBox *hbox;
    LBox *vbox;
    Label *boundsLabel;
    Label *test1Label;
    Label *test2Label;
    Button *testButton;
    TextBox *testTextBox;
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

static void handleTextChanged(void *selfPtr, EventArgs *args)
{
    (void)selfPtr;
    const char *newText = EventArgs_evInfo(args);
    printf("new text: %s\n", newText);
    fflush(stdout);
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

    item = MenuItem_create("&File");
    MenuItem_setSubMenu(item, fileMenu);
    Menu_addItem(self->mainMenu, item);

    self->mainWindow = Window_create("POCAS Test", 480, 160);
    Window_setMenu(self->mainWindow, self->mainMenu);

    self->vbox = LBox_create(BO_Vertical);
    self->hbox = LBox_create(BO_Horizontal);

    Extents margin = {2,2,2,2};

    self->test1Label = Label_create("(1) This is a test!");
    Control_show(self->test1Label);
    Control_setMargin(self->test1Label, &margin);
    LBox_addControl(self->hbox, self->test1Label);
    self->testButton = Button_create("Exit");
    Button_setCommand(self->testButton, self->closeCommand);
    Control_show(self->testButton);
    Control_setMargin(self->testButton, &margin);
    LBox_addControl(self->hbox, self->testButton);
    self->testTextBox = TextBox_create(TBS_Normal);
    Control_show(self->testTextBox);
    Control_setMargin(self->testTextBox, &margin);
    LBox_addControl(self->hbox, self->testTextBox);
    self->test2Label = Label_create("(2) ♫ 42");
    Control_show(self->test2Label);
    Control_setMargin(self->test2Label, &margin);
    LBox_addControl(self->hbox, self->test2Label);

    margin.bottom = 24;
    self->boundsLabel = Label_create(0);
    Control_show(self->boundsLabel);
    Control_setMargin(self->boundsLabel, &margin);
    LBox_addControl(self->vbox, self->boundsLabel);
    LBox_addControl(self->vbox, self->hbox);

    Container_setControl(self->mainWindow, self->vbox);

    Event_register(Command_invokedEvent(self->closeCommand),
            self, handleCloseCommand);

    Event_register(Window_closingEvent(self->mainWindow),
            self, handleWindowClosing);

    Event_register(Container_resizedEvent(self->mainWindow),
            self, handleContainerResized);

    Event_register(TextBox_textChangedEvent(self->testTextBox),
            self, handleTextChanged);

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

    Event_unregister(Command_invokedEvent(self->closeCommand),
            self, handleCloseCommand);

    Event_unregister(Window_closingEvent(self->mainWindow),
            self, handleWindowClosing);

    Event_unregister(Container_resizedEvent(self->mainWindow),
            self, handleContainerResized);

    Event_unregister(TextBox_textChangedEvent(self->testTextBox),
            self, handleTextChanged);

    LBox_removeControl(self->hbox, self->test2Label);
    LBox_removeControl(self->hbox, self->testTextBox);
    LBox_removeControl(self->hbox, self->testButton);
    LBox_removeControl(self->hbox, self->test1Label);
    LBox_removeControl(self->vbox, self->hbox);
    LBox_removeControl(self->vbox, self->boundsLabel);
    Container_setControl(self->mainWindow, 0);

    TextBox_destroy(self->testTextBox);
    Button_destroy(self->testButton);
    Label_destroy(self->test2Label);
    Label_destroy(self->test1Label);
    Label_destroy(self->boundsLabel);
    LBox_destroy(self->hbox);
    LBox_destroy(self->vbox);

    Menu_destroy(self->mainMenu);
    Command_destroy(self->closeCommand);
    Window_destroy(self->mainWindow);

    self->disposed = 1;
}

SOLOCAL void Gui_destroy(Gui *self)
{
    if (!self) return;
    Gui_dispose(self);
    free(self);
}
