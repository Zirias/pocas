#ifndef POCAS_GUI_PRIVATE_BACKEND_H
#define POCAS_GUI_PRIVATE_BACKEND_H

#include <pocas/gui/backend.h>
#include <pocas/gui/button.h>
#include <pocas/gui/messagebox.h>

typedef struct Bounds Bounds;
typedef struct Event Event;
typedef struct Window Window;
typedef struct Menu Menu;
typedef struct MenuItem MenuItem;
typedef struct Command Command;
typedef struct Label Label;
typedef struct TextBox TextBox;

typedef struct IPrivateControl
{
    int (*create)(void *self);
    void *(*container)(const void *self);
    void (*bounds)(const void *self, Bounds *b);
    int (*shown)(const void *self);
    void (*setContainer)(void *self, void *container);
    void (*setContentSize)(void *self,
            unsigned int width, unsigned int height);
    void (*destroy)(void *self);
} IPrivateControl;

typedef struct IPrivateContainer
{
    int (*create)(void *self);
    int (*setBounds)(void *self, Bounds *b);
    void (*destroy)(void *self);
} IPrivateContainer;

typedef struct IPrivateWindow
{
    void (*close)(Window *self);
    const char *(*title)(const Window *self);
    Window *(*parent)(const Window *self);
    int (*width)(const Window *self);
    int (*height)(const Window *self);
    Event *(*lastWindowClosedEvent)(void);
} IPrivateWindow;

typedef struct IPrivateMenuItem
{
    Menu *(*subMenu)(const MenuItem *self);
    const char *(*text)(const MenuItem *self);
    void (*select)(MenuItem *self);
} IPrivateMenuItem;

typedef struct IPrivateLabel
{
    const char *(*text)(const Label *self);
} IPrivateLabel;

typedef struct IPrivateButton
{
    const char *(*text)(const Button *self);
    ButtonStyle (*style)(const Button *self);
    void (*click)(Button *self);
} IPrivateButton;

typedef struct GuiPrivateApi
{
    void *(*backendObject)(const void *frontendObject);
    int (*setBackendObject)(void *frontendObject, void *backendObject);
    void *(*controlObject)(const void *frontendObject);
    int (*setControlObject)(void *frontendObject, void *controlObject);
    void *(*containerObject)(const void *frontendObject);
    int (*setContainerObject)(void *frontendObject, void *containerObject);

    IPrivateControl control;
    IPrivateContainer container;
    IPrivateWindow window;
    IPrivateMenuItem menuItem;
    IPrivateLabel label;
    IPrivateButton button;
} GuiPrivateApi;

typedef struct IBackendControl
{
    void (*setContainer)(void *control, void *container);
    void (*setBounds)(void *control, const Bounds *b);
    void (*setShown)(void *control, int shown);
    void (*setEnabled)(void *control, int enabled);
    void (*focus)(void *control);
} IBackendControl;

typedef struct IBackendWindow
{
    int (*create)(Window *self);
    void (*setMenu)(Window *self, Menu *menu);
    void (*close)(Window *self);
    void (*destroy)(Window *self);
} IBackendWindow;

typedef struct IBackendMenu
{
    int (*create)(Menu *self);
    void (*addItem)(Menu *self, MenuItem *item);
    void (*removeItem)(Menu *self, MenuItem *item);
    void (*destroy)(Menu *self);
} IBackendMenu;

typedef struct IBackendMenuItem
{
    int (*create)(MenuItem *self);
    void (*destroy)(MenuItem *self);
} IBackendMenuItem;

typedef struct IBackendMessageBox
{
    MessageBoxButton (*show)(const Window *w, const char *title,
            const char *text, MessageBoxButton buttons, MessageBoxStyle style);
} IBackendMessageBox;

typedef struct IBackendLabel
{
    int (*create)(Label *self);
    void (*setText)(Label *self, const char *text);
    void (*destroy)(Label *self);
} IBackendLabel;

typedef struct IBackendButton
{
    int (*create)(Button *self);
    void (*setText)(Button *self, const char *text);
    void (*destroy)(Button *self);
} IBackendButton;

typedef void (*TextBox_textChanged)(TextBox *self, const char *text);
typedef struct IBackendTextBox
{
    int (*create)(TextBox *self, TextBox_textChanged changed);
    void (*setText)(TextBox *self, const char *text);
    void (*destroy)(TextBox *self);
} IBackendTextBox;

typedef struct GuiBackendApi
{
    const char *(*name)(void);
    IBackendControl control;
    IBackendWindow window;
    IBackendMenu menu;
    IBackendMenuItem menuItem;
    IBackendMessageBox messageBox;
    IBackendLabel label;
    IBackendButton button;
    IBackendTextBox textBox;
} GuiBackendApi;

struct Backend
{
    GuiBackendApi backendApi;
    const GuiPrivateApi *privateApi;
};

#endif
