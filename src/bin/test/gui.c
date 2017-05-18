#include <stdlib.h>
#include <stdio.h>

#include <pocas/core/event.h>
#include <pocas/core/eventloop.h>

#include <pocas/gui/bounds.h>
#include <pocas/gui/button.h>
#include <pocas/gui/container.h>
#include <pocas/gui/control.h>
#include <pocas/gui/extents.h>
#include <pocas/gui/boxlayout.h>
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
    PG_Window *mainWindow;
    PG_Menu *mainMenu;
    PG_BoxLayout *hbox;
    PG_BoxLayout *vbox;
    PG_Label *boundsLabel;
    PG_Label *test1Label;
    PG_Label *test2Label;
    PG_Button *testButton;
    PG_TextBox *testTextBox;

    PG_Window *aboutBox;

    PG_Window *changeButtonText;
    PG_BoxLayout *dlgBox;
    PG_Label *dlgLabel;
    PG_TextBox *dlgTextBox;
    PG_Button *dlgButton;
};

static void handleExitSelected(void *selfPtr, PC_EventArgs *args)
{
    (void)args;
    Gui *self = selfPtr;
    PG_Window_close(self->mainWindow);
}

static void handleWindowClosing(void *selfPtr, PC_EventArgs *args)
{
    Gui *self = selfPtr;

    PG_MessageBoxButton result = PG_MessageBox_show(self->mainWindow, "Really quit?",
            "Are you sure you want to quit?", PG_MBB_Yes|PG_MBB_No, PG_MBS_Warning);

    if (result == PG_MBB_No)
    {
        args->handled = 1;
    }
}

static void handleTextChanged(void *selfPtr, PC_EventArgs *args)
{
    (void)selfPtr;
    const char *newText = args->evInfo;
    printf("new text: %s\n", newText);
    fflush(stdout);
}

static void showSize(Gui *self, PG_Bounds *b)
{
    char labeltext[20];
    snprintf(labeltext, 20, "[ %u x %u ]", b->width, b->height);
    PG_Label_setText(self->boundsLabel, labeltext);
}

static void handleContainerResized(void *selfPtr, PC_EventArgs *args)
{
    Gui *self = selfPtr;
    PG_Bounds *b = args->evInfo;
    showSize(self, b);
}

static void showAboutBox(void *selfPtr, PC_EventArgs *args)
{
    (void)args;
    Gui *self = selfPtr;
    PG_Control_show(self->aboutBox);
}

static void hideSender(void *selfPtr, PC_EventArgs *args)
{
    (void)selfPtr;
    PG_Control_hide(args->sender);
    args->handled = 1;
}

static void showChangeDialog(void *selfPtr, PC_EventArgs *args)
{
    (void)args;
    Gui *self = selfPtr;

    PG_TextBox_setText(self->dlgTextBox, PG_Button_text(self->testButton));
    const char *text = PG_Window_executeDialog(self->changeButtonText);
    if (text && *text)
    {
        PG_Button_setText(self->testButton, text);
    }
}

static void hideChangeDialog(void *selfPtr, PC_EventArgs *args)
{
    Gui *self = selfPtr;

    const char *text = 0;
    if (args->sender == self->dlgButton)
    {
        text = PG_TextBox_text(self->dlgTextBox);
    }
    PG_Window_closeDialog(self->changeButtonText, (char *)text);
    args->handled = 1;
}

static void initChangeDialog(void *selfPtr, PC_EventArgs *args)
{
    (void)args;
    Gui *self = selfPtr;
    PG_Control_focus(self->dlgTextBox);
}

SOLOCAL Gui *Gui_create(void)
{
    Gui *self = malloc(sizeof(Gui));
    self->disposed = 0;

    self->mainMenu = PG_Menu_create();
    PG_Menu *fileMenu = PG_Menu_create();

    PG_MenuItem *item = PG_MenuItem_create("&About");
    PC_Event_register(PG_MenuItem_selectedEvent(item), self, showAboutBox);
    PG_Menu_addItem(fileMenu, item);

    item = PG_MenuItem_create("E&xit");
    PC_Event_register(PG_MenuItem_selectedEvent(item), self, handleExitSelected);
    PG_Menu_addItem(fileMenu, item);

    item = PG_MenuItem_create("&File");
    PG_MenuItem_setSubMenu(item, fileMenu);
    PG_Menu_addItem(self->mainMenu, item);

    self->mainWindow = PG_Window_create(0, "POCAS Test", 480, 160);
    PG_Window_setMenu(self->mainWindow, self->mainMenu);

    self->vbox = PG_BoxLayout_create(PG_BO_Vertical);
    self->hbox = PG_BoxLayout_create(PG_BO_Horizontal);

    PG_Extents margin = {2,2,2,2};

    self->test1Label = PG_Label_create("(1) This is a test!");
    PG_Control_show(self->test1Label);
    PG_Control_setMargin(self->test1Label, &margin);
    PG_BoxLayout_addControl(self->hbox, self->test1Label);
    self->testButton = PG_Button_create(PG_BS_Normal, "Change me");
    PG_Control_show(self->testButton);
    PG_Control_setMargin(self->testButton, &margin);
    PG_BoxLayout_addControl(self->hbox, self->testButton);
    self->testTextBox = PG_TextBox_create(PG_TBS_Normal);
    PG_Control_show(self->testTextBox);
    PG_Control_setMargin(self->testTextBox, &margin);
    PG_BoxLayout_addControl(self->hbox, self->testTextBox);
    self->test2Label = PG_Label_create("(2) ♫ 42");
    PG_Control_show(self->test2Label);
    PG_Control_setMargin(self->test2Label, &margin);
    PG_BoxLayout_addControl(self->hbox, self->test2Label);

    margin.bottom = 24;
    self->boundsLabel = PG_Label_create(0);
    PG_Control_show(self->boundsLabel);
    PG_Control_setMargin(self->boundsLabel, &margin);
    PG_BoxLayout_addControl(self->vbox, self->boundsLabel);
    PG_BoxLayout_addControl(self->vbox, self->hbox);

    PG_Container_setControl(self->mainWindow, self->vbox);

    PC_Event_register(PG_Button_clickedEvent(self->testButton),
            self, showChangeDialog);

    PC_Event_register(PG_Window_closingEvent(self->mainWindow),
            self, handleWindowClosing);

    PC_Event_register(PG_Container_resizedEvent(self->mainWindow),
            self, handleContainerResized);

    PC_Event_register(PG_TextBox_textChangedEvent(self->testTextBox),
            self, handleTextChanged);

    self->aboutBox = PG_Window_create(self->mainWindow,
            "About POCAS Test", 320, 200);

    PC_Event_register(PG_Window_closingEvent(self->aboutBox),
            self, hideSender);

    self->changeButtonText = PG_Window_create(self->mainWindow,
            "Change button text", 360, 60);
    margin.bottom = 2;
    margin.left = 4;
    self->dlgBox = PG_BoxLayout_create(PG_BO_Horizontal);
    self->dlgLabel = PG_Label_create("Text:");
    PG_Control_show(self->dlgLabel);
    PG_Control_setMargin(self->dlgLabel, &margin);
    PG_BoxLayout_addControl(self->dlgBox, self->dlgLabel);
    self->dlgTextBox = PG_TextBox_create(PG_TBS_Normal);
    PG_Control_show(self->dlgTextBox);
    PG_Control_setMinSize(self->dlgTextBox, 200, 0);
    PG_Control_setMargin(self->dlgTextBox, &margin);
    PG_BoxLayout_addControl(self->dlgBox, self->dlgTextBox);
    self->dlgButton = PG_Button_create(PG_BS_Default, "Apply");
    PG_Control_show(self->dlgButton);
    PG_Control_setMargin(self->dlgButton, &margin);
    PG_BoxLayout_addControl(self->dlgBox, self->dlgButton);
    PG_Container_setControl(self->changeButtonText, self->dlgBox);

    PC_Event_register(PG_Button_clickedEvent(self->dlgButton),
            self, hideChangeDialog);

    PC_Event_register(PG_Window_closingEvent(self->changeButtonText),
            self, hideChangeDialog);

    PC_Event_register(PG_Window_dialogShownEvent(self->changeButtonText),
            self, initChangeDialog);

    return self;
}

SOLOCAL int Gui_run(Gui *self)
{
    PG_Control_show(self->mainWindow);
    PG_Bounds b;
    PG_Container_bounds(self->mainWindow, &b);
    showSize(self, &b);
    return PC_EventLoop_run();
}

SOLOCAL void Gui_dispose(Gui *self)
{
    if (self->disposed) return;

    PG_BoxLayout_removeControl(self->dlgBox, self->dlgButton);
    PG_BoxLayout_removeControl(self->dlgBox, self->dlgTextBox);
    PG_BoxLayout_removeControl(self->dlgBox, self->dlgLabel);
    PG_Container_setControl(self->changeButtonText, 0);

    PG_Button_destroy(self->dlgButton);
    PG_TextBox_destroy(self->dlgTextBox);
    PG_Label_destroy(self->dlgLabel);
    PG_BoxLayout_destroy(self->dlgBox);

    PG_Window_destroy(self->changeButtonText);

    PG_Window_destroy(self->aboutBox);

    PG_BoxLayout_removeControl(self->hbox, self->test2Label);
    PG_BoxLayout_removeControl(self->hbox, self->testTextBox);
    PG_BoxLayout_removeControl(self->hbox, self->testButton);
    PG_BoxLayout_removeControl(self->hbox, self->test1Label);
    PG_BoxLayout_removeControl(self->vbox, self->hbox);
    PG_BoxLayout_removeControl(self->vbox, self->boundsLabel);
    PG_Container_setControl(self->mainWindow, 0);

    PG_TextBox_destroy(self->testTextBox);
    PG_Button_destroy(self->testButton);
    PG_Label_destroy(self->test2Label);
    PG_Label_destroy(self->test1Label);
    PG_Label_destroy(self->boundsLabel);
    PG_BoxLayout_destroy(self->hbox);
    PG_BoxLayout_destroy(self->vbox);

    PG_Menu_destroy(self->mainMenu);
    PG_Window_destroy(self->mainWindow);

    self->disposed = 1;
}

SOLOCAL void Gui_destroy(Gui *self)
{
    if (!self) return;
    Gui_dispose(self);
    free(self);
}
