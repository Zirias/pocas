#include <stdlib.h>
#include <stdio.h>

#include <pocas/core/event.h>
#include <pocas/core/eventloop.h>

#include <pocas/gui/bounds.h>
#include <pocas/gui/button.h>
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
    LBox *hbox;
    LBox *vbox;
    Label *boundsLabel;
    Label *test1Label;
    Label *test2Label;
    Button *testButton;
    TextBox *testTextBox;

    Window *aboutBox;

    Window *changeButtonText;
    LBox *dlgBox;
    Label *dlgLabel;
    TextBox *dlgTextBox;
    Button *dlgButton;
};

static void handleExitSelected(void *selfPtr, EventArgs *args)
{
    (void)args;
    Gui *self = selfPtr;
    Window_close(self->mainWindow);
}

static void handleWindowClosing(void *selfPtr, EventArgs *args)
{
    Gui *self = selfPtr;

    MessageBoxButton result = MessageBox_show(self->mainWindow, "Really quit?",
            "Are you sure you want to quit?", MBB_Yes|MBB_No, MBS_Warning);

    if (result == MBB_No)
    {
	args->handled = 1;
    }
}

static void handleTextChanged(void *selfPtr, EventArgs *args)
{
    (void)selfPtr;
    const char *newText = args->evInfo;
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
    Bounds *b = args->evInfo;
    showSize(self, b);
}

static void showAboutBox(void *selfPtr, EventArgs *args)
{
    (void)args;
    Gui *self = selfPtr;
    Control_show(self->aboutBox);
}

static void hideSender(void *selfPtr, EventArgs *args)
{
    (void)selfPtr;
    Control_hide(args->sender);
    args->handled = 1;
}

static void showChangeDialog(void *selfPtr, EventArgs *args)
{
    (void)args;
    Gui *self = selfPtr;

    TextBox_setText(self->dlgTextBox, Button_text(self->testButton));
    const char *text = Window_showDialog(self->changeButtonText);
    if (text)
    {
        Button_setText(self->testButton, text);
    }
}

static void hideChangeDialog(void *selfPtr, EventArgs *args)
{
    Gui *self = selfPtr;

    const char *text = 0;
    if (args->sender == self->dlgButton)
    {
        text = TextBox_text(self->dlgTextBox);
    }
    Window_closeDialog(self->changeButtonText, (char *)text);
    args->handled = 1;
}

static void initChangeDialog(void *selfPtr, EventArgs *args)
{
    (void)args;
    Gui *self = selfPtr;
    Control_focus(self->dlgTextBox);
}

SOLOCAL Gui *Gui_create(void)
{
    Gui *self = malloc(sizeof(Gui));
    self->disposed = 0;

    self->mainMenu = Menu_create();
    Menu *fileMenu = Menu_create();

    MenuItem *item = MenuItem_create("&About");
    Event_register(MenuItem_selectedEvent(item), self, showAboutBox);
    Menu_addItem(fileMenu, item);

    item = MenuItem_create("E&xit");
    Event_register(MenuItem_selectedEvent(item), self, handleExitSelected);
    Menu_addItem(fileMenu, item);

    item = MenuItem_create("&File");
    MenuItem_setSubMenu(item, fileMenu);
    Menu_addItem(self->mainMenu, item);

    self->mainWindow = Window_create(0, "POCAS Test", 480, 160);
    Window_setMenu(self->mainWindow, self->mainMenu);

    self->vbox = LBox_create(BO_Vertical);
    self->hbox = LBox_create(BO_Horizontal);

    Extents margin = {2,2,2,2};

    self->test1Label = Label_create("(1) This is a test!");
    Control_show(self->test1Label);
    Control_setMargin(self->test1Label, &margin);
    LBox_addControl(self->hbox, self->test1Label);
    self->testButton = Button_create(BS_Normal, "Change me");
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

    Event_register(Button_clickedEvent(self->testButton),
            self, showChangeDialog);

    Event_register(Window_closingEvent(self->mainWindow),
            self, handleWindowClosing);

    Event_register(Container_resizedEvent(self->mainWindow),
            self, handleContainerResized);

    Event_register(TextBox_textChangedEvent(self->testTextBox),
            self, handleTextChanged);

    self->aboutBox = Window_create(self->mainWindow,
            "About POCAS Test", 320, 200);

    Event_register(Window_closingEvent(self->aboutBox),
            self, hideSender);

    self->changeButtonText = Window_create(self->mainWindow,
            "Change button text", 360, 60);
    margin.bottom = 2;
    margin.left = 4;
    self->dlgBox = LBox_create(BO_Horizontal);
    self->dlgLabel = Label_create("Text:");
    Control_show(self->dlgLabel);
    Control_setMargin(self->dlgLabel, &margin);
    LBox_addControl(self->dlgBox, self->dlgLabel);
    self->dlgTextBox = TextBox_create(TBS_Normal);
    Control_show(self->dlgTextBox);
    Control_setMinSize(self->dlgTextBox, 200, 0);
    Control_setMargin(self->dlgTextBox, &margin);
    LBox_addControl(self->dlgBox, self->dlgTextBox);
    self->dlgButton = Button_create(BS_Default, "Apply");
    Control_show(self->dlgButton);
    Control_setMargin(self->dlgButton, &margin);
    LBox_addControl(self->dlgBox, self->dlgButton);
    Container_setControl(self->changeButtonText, self->dlgBox);

    Event_register(Button_clickedEvent(self->dlgButton),
            self, hideChangeDialog);

    Event_register(Window_closingEvent(self->changeButtonText),
            self, hideChangeDialog);

    Event_register(Window_dialogShownEvent(self->changeButtonText),
            self, initChangeDialog);

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

    LBox_removeControl(self->dlgBox, self->dlgButton);
    LBox_removeControl(self->dlgBox, self->dlgTextBox);
    LBox_removeControl(self->dlgBox, self->dlgLabel);
    Container_setControl(self->changeButtonText, 0);

    Button_destroy(self->dlgButton);
    TextBox_destroy(self->dlgTextBox);
    Label_destroy(self->dlgLabel);
    LBox_destroy(self->dlgBox);

    Window_destroy(self->changeButtonText);

    Window_destroy(self->aboutBox);

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
    Window_destroy(self->mainWindow);

    self->disposed = 1;
}

SOLOCAL void Gui_destroy(Gui *self)
{
    if (!self) return;
    Gui_dispose(self);
    free(self);
}
