#ifndef POCAS_GUI_PRIVATE_BACKEND_H
#define POCAS_GUI_PRIVATE_BACKEND_H

#include <pocas/gui/backend.h>
#include <pocas/gui/button.h>
#include <pocas/gui/messagebox.h>

C_CLASS_DECL(PG_Bounds);
C_CLASS_DECL(PC_Event);
C_CLASS_DECL(PG_Window);
C_CLASS_DECL(PG_Menu);
C_CLASS_DECL(PG_MenuItem);
C_CLASS_DECL(PG_Command);
C_CLASS_DECL(PG_Label);
C_CLASS_DECL(PG_TextBox);

C_CLASS_DECL(PG_IPrivateControl);
struct PG_IPrivateControl
{
    int (*create)(void *self);
    void *(*container)(const void *self);
    void (*bounds)(const void *self, PG_Bounds *b);
    int (*shown)(const void *self);
    void (*setContainer)(void *self, void *container);
    void (*setContentSize)(void *self,
            unsigned int width, unsigned int height);
    void (*destroy)(void *self);
};

C_CLASS_DECL(PG_IPrivateContainer);
struct PG_IPrivateContainer
{
    int (*create)(void *self);
    int (*setBounds)(void *self, PG_Bounds *b);
    void (*destroy)(void *self);
};

C_CLASS_DECL(PG_IPrivateWindow);
struct PG_IPrivateWindow
{
    void (*close)(PG_Window *self);
    const char *(*title)(const PG_Window *self);
    PG_Window *(*parent)(const PG_Window *self);
    int (*width)(const PG_Window *self);
    int (*height)(const PG_Window *self);
    PC_Event *(*lastWindowClosedEvent)(void);
};

C_CLASS_DECL(PG_IPrivateMenuItem);
struct PG_IPrivateMenuItem
{
    PG_Menu *(*subMenu)(const PG_MenuItem *self);
    const char *(*text)(const PG_MenuItem *self);
    void (*select)(PG_MenuItem *self);
};

C_CLASS_DECL(PG_IPrivateLabel);
struct PG_IPrivateLabel
{
    const char *(*text)(const PG_Label *self);
};

C_CLASS_DECL(PG_IPrivateButton);
struct PG_IPrivateButton
{
    const char *(*text)(const PG_Button *self);
    PG_ButtonStyle (*style)(const PG_Button *self);
    void (*click)(PG_Button *self);
};

C_CLASS_DECL(PG_PrivateApi);
struct PG_PrivateApi
{
    void *(*backendObject)(const void *frontendObject);
    int (*setBackendObject)(void *frontendObject, void *backendObject);
    void *(*controlObject)(const void *frontendObject);
    int (*setControlObject)(void *frontendObject, void *controlObject);
    void *(*containerObject)(const void *frontendObject);
    int (*setContainerObject)(void *frontendObject, void *containerObject);

    PG_IPrivateControl control;
    PG_IPrivateContainer container;
    PG_IPrivateWindow window;
    PG_IPrivateMenuItem menuItem;
    PG_IPrivateLabel label;
    PG_IPrivateButton button;
};

C_CLASS_DECL(PG_IBackendControl);
struct PG_IBackendControl
{
    void (*setContainer)(void *control, void *container);
    void (*setBounds)(void *control, const PG_Bounds *b);
    void (*setShown)(void *control, int shown);
    void (*setEnabled)(void *control, int enabled);
    void (*focus)(void *control);
};

C_CLASS_DECL(PG_IBackendWindow);
struct PG_IBackendWindow
{
    int (*create)(PG_Window *self);
    void (*setMenu)(PG_Window *self, PG_Menu *menu);
    void (*close)(PG_Window *self);
    void (*destroy)(PG_Window *self);
};

C_CLASS_DECL(PG_IBackendMenu);
struct PG_IBackendMenu
{
    int (*create)(PG_Menu *self);
    void (*addItem)(PG_Menu *self, PG_MenuItem *item);
    void (*removeItem)(PG_Menu *self, PG_MenuItem *item);
    void (*destroy)(PG_Menu *self);
};

C_CLASS_DECL(PG_IBackendMenuItem);
struct PG_IBackendMenuItem
{
    int (*create)(PG_MenuItem *self);
    void (*destroy)(PG_MenuItem *self);
};

C_CLASS_DECL(PG_IBackendMessageBox);
struct PG_IBackendMessageBox
{
    PG_MessageBoxButton (*show)(const PG_Window *w, const char *title,
            const char *text, PG_MessageBoxButton buttons, PG_MessageBoxStyle style);
};

C_CLASS_DECL(PG_IBackendLabel);
struct PG_IBackendLabel
{
    int (*create)(PG_Label *self);
    void (*setText)(PG_Label *self, const char *text);
    void (*destroy)(PG_Label *self);
};

C_CLASS_DECL(PG_IBackendButton);
struct PG_IBackendButton
{
    int (*create)(PG_Button *self);
    void (*setText)(PG_Button *self, const char *text);
    void (*destroy)(PG_Button *self);
};

typedef void (*PG_TextBox_textChanged)(PG_TextBox *self, const char *text);
C_CLASS_DECL(PG_IBackendTextBox);
struct PG_IBackendTextBox
{
    int (*create)(PG_TextBox *self, PG_TextBox_textChanged changed);
    void (*setText)(PG_TextBox *self, const char *text);
    void (*destroy)(PG_TextBox *self);
};

C_CLASS_DECL(PG_BackendApi);
struct PG_BackendApi
{
    const char *(*name)(void);
    PG_IBackendControl control;
    PG_IBackendWindow window;
    PG_IBackendMenu menu;
    PG_IBackendMenuItem menuItem;
    PG_IBackendMessageBox messageBox;
    PG_IBackendLabel label;
    PG_IBackendButton button;
    PG_IBackendTextBox textBox;
};

struct PG_Backend
{
    PG_BackendApi backendApi;
    const PG_PrivateApi *privateApi;
};

#endif
