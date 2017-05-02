#ifndef POCAS_GUI_PRIVATE_BACKEND_H
#define POCAS_GUI_PRIVATE_BACKEND_H

#include <pocas/gui/backend.h>
#include <pocas/gui/messagebox.h>

typedef struct IPrivateControl
{
    int (*create)(void *self);
    void *(*container)(const void *self);
    void (*setContainer)(void *self, void *container);
    void (*destroy)(void *self);
} IPrivateControl;

typedef struct IPrivateContainer
{
    int (*create)(void *self);
    void (*setWidth)(void *self, unsigned int width);
    void (*setHeight)(void *self, unsigned int height);
    void (*destroy)(void *self);
} IPrivateContainer;

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
} GuiPrivateApi;

typedef struct Window Window;
typedef struct Menu Menu;
typedef struct MenuItem MenuItem;
typedef struct Command Command;
typedef struct Label Label;

typedef struct IBackendControl
{
    void (*setContainer)(void *control, void *container);
} IBackendControl;

typedef struct IBackendWindow
{
    int (*create)(Window *self);
    void (*show)(Window *self);
    void (*hide)(Window *self);
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

typedef struct IBackendCommand
{
    int (*create)(Command *self);
    void (*destroy)(Command *self);
} IBackendCommand;

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

typedef struct GuiBackendApi
{
    const char *(*name)(void);
    IBackendControl control;
    IBackendWindow window;
    IBackendMenu menu;
    IBackendMenuItem menuItem;
    IBackendCommand command;
    IBackendMessageBox messageBox;
    IBackendLabel label;
} GuiBackendApi;

struct Backend
{
    GuiBackendApi backendApi;
    const GuiPrivateApi *privateApi;
};

#endif
