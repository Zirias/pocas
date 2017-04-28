#include "c11threads.h"

#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>

#include <pocas/core/event_win32.h>
#include <pocas/core/eventloop_win32.h>
#include <pocas/core/hashtable.h>

#include <pocas/gui/private/backend.h>
#include <pocas/gui/bounds.h>
#include <pocas/gui/command.h>
#include <pocas/gui/container.h>
#include <pocas/gui/window.h>
#include <pocas/gui/menu.h>
#include <pocas/gui/label.h>

struct bdata
{
    HashTable *commands;
    WORD nextCommandId;
    size_t nWindows;
};

enum B_Type
{
    BT_Window,
    BT_Menu,
    BT_MenuItem,
    BT_Command,
    BT_Label,
};

typedef struct BO
{
    enum B_Type t;
} BO;

typedef struct B_Window
{
    BO bo;
    Window *w;
    WNDCLASSEXW wc;
    HWND hndl;
    LPWSTR name;
} B_Window;

typedef struct B_Menu
{
    BO bo;
    Menu *m;
    HMENU menu;
} B_Menu;

typedef struct B_MenuItem
{
    BO bo;
    MenuItem *m;
    LPWSTR text;
} B_MenuItem;

typedef struct B_Command
{
    BO bo;
    Command *c;
    WORD id;
} B_Command;

typedef struct B_Label
{
    BO bo;
    Label *l;
    HWND hndl;
    LPWSTR text;
} B_Label;

static thread_local struct bdata bdata = {0, 0x100, 0};

static const char *B_name(void)
{
    return "win32";
}

SOLOCAL Backend *defaultBackend;

static void updateWindowClientSize(B_Window *self)
{
    RECT r;
    Bounds b;
    GetClientRect(self->hndl, &r);

    b.x = 0;
    b.y = 0;
    b.width = (unsigned int) r.right;
    b.height = (unsigned int) r.bottom;
    const GuiPrivateApi *api = defaultBackend->privateApi;
    api->container.setHeight(self->w, b.height);
    api->container.setWidth(self->w, b.width);

    Event *resized = Container_resizedEvent(self->w);
    EventArgs *args = EventArgs_create(resized, self->w, &b);
    Event_raise(resized, args);
    EventArgs_destroy(args);
}

static void handleWin32MessageEvent(void *w, EventArgs *args)
{
    B_Window *self = w;
    Win32MsgEvInfo *mei = EventArgs_evInfo(args);
    if (mei->wnd != self->hndl) return;

    WORD id;
    switch (mei->msg)
    {
    case WM_SIZE:
        updateWindowClientSize(self);
        EventArgs_setHandled(args);
        break;

    case WM_CLOSE:
        Window_close(self->w);
        EventArgs_setHandled(args);
        break;

    case WM_COMMAND:
        id = LOWORD(mei->wp);
        const B_Command *c = HashTable_get(bdata.commands, &id);
        if (c)
        {
            Command_invoke(c->c);
            EventArgs_setHandled(args);
        }
        break;

    case WM_DESTROY:
        EventLoop_setProcessMessages(--bdata.nWindows);
        if (!bdata.nWindows)
        {
            Event *lwc = Window_lastWindowClosedEvent();
            EventArgs *lwcArgs = EventArgs_create(lwc, 0, 0);
            Event_raise(lwc, lwcArgs);
            if (!EventArgs_handled(lwcArgs))
            {
                PostQuitMessage(0);
            }
            EventArgs_destroy(lwcArgs);
        }
        EventArgs_setHandled(args);
        break;
    }
}

static int B_Window_create(Window *self)
{
    B_Window *bw = calloc(1, sizeof(B_Window));
    defaultBackend->privateApi->setBackendObject(self, bw);
    bw->bo.t = BT_Window;
    const char *title = Window_title(self);
    size_t titleLen = strlen(title) + 1;
    bw->name = calloc(1, 2 * titleLen);
    MultiByteToWideChar(CP_UTF8, 0, title, titleLen, bw->name, titleLen);
    titleLen = 2 * (wcslen(bw->name) + 1);
    bw->name = realloc(bw->name, titleLen);
    bw->w = self;
    bw->wc.cbSize = sizeof(WNDCLASSEXW);
    bw->wc.hInstance = GetModuleHandleW(0);
    bw->wc.lpszClassName = bw->name;
    bw->wc.lpfnWndProc = EventLoop_win32WndProc;
    bw->wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
    bw->wc.hCursor = LoadCursorA(0, IDC_ARROW);
    RegisterClassExW(&bw->wc);
    bw->hndl = CreateWindowExW(0, bw->name, bw->name,
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
            Window_width(self), Window_height(self), 0, 0,
            bw->wc.hInstance, 0);
    Event_register(EventLoop_win32MsgEvent(), bw, handleWin32MessageEvent);
    EventLoop_setProcessMessages(++bdata.nWindows);
    return 1;
}

static void B_Window_show(Window *self)
{
    B_Window *bw = defaultBackend->privateApi->backendObject(self);
    ShowWindow(bw->hndl, SW_SHOWNORMAL);
    UpdateWindow(bw->hndl);
}

static void B_Window_hide(Window *self)
{
    B_Window *bw = defaultBackend->privateApi->backendObject(self);
    ShowWindow(bw->hndl, SW_HIDE);
}

static void B_Window_setMenu(Window *self, Menu *menu)
{
    B_Window *bw = defaultBackend->privateApi->backendObject(self);
    B_Menu *bm = defaultBackend->privateApi->backendObject(menu);
    SetMenu(bw->hndl, bm->menu);
    DrawMenuBar(bw->hndl);
}

static void B_Window_close(Window *self)
{
    B_Window *bw = defaultBackend->privateApi->backendObject(self);
    DestroyWindow(bw->hndl);
}

static void B_Window_destroy(Window *self)
{
    if (!self) return;
    B_Window *bw = defaultBackend->privateApi->backendObject(self);
    DestroyWindow(bw->hndl);
    UnregisterClassW(bw->name, bw->wc.hInstance);
    Event_unregister(EventLoop_win32MsgEvent(), bw, handleWin32MessageEvent);
    free(bw->name);
    free(bw);
}

static int B_Menu_create(Menu *self)
{
    B_Menu *bm = malloc(sizeof(B_Menu));
    bm->bo.t = BT_Menu;
    defaultBackend->privateApi->setBackendObject(self, bm);
    bm->m = self;
    bm->menu = CreateMenu();
    return 1;
}

static void B_Menu_addItem(Menu *self, MenuItem *item)
{
    B_Menu *bm = defaultBackend->privateApi->backendObject(self);
    B_MenuItem *bi = defaultBackend->privateApi->backendObject(item);
    Menu *subMenu = MenuItem_subMenu(item);
    if (subMenu)
    {
        B_Menu *sub = defaultBackend->privateApi->backendObject(subMenu);
        AppendMenuW(bm->menu, MF_POPUP, (UINT_PTR)sub->menu, bi->text);
    }
    else
    {
        Command *cmd = MenuItem_command(item);
        if (cmd)
        {
            B_Command *c = defaultBackend->privateApi->backendObject(cmd);
            AppendMenuW(bm->menu, MF_STRING, (UINT_PTR)c->id, bi->text);
        }
        else
        {
            AppendMenuW(bm->menu, MF_STRING, 0, bi->text);
        }
    }
}

static void B_Menu_destroy(Menu *self)
{
    if (!self) return;
    B_Menu *bm = defaultBackend->privateApi->backendObject(self);
    DestroyMenu(bm->menu);
    free(bm);
}

static void B_MenuItem_updateText(B_MenuItem *self)
{
    free(self->text);
    const char *text = MenuItem_text(self->m);
    size_t textLen = strlen(text) + 1;
    self->text = calloc(1, 2 * textLen);
    MultiByteToWideChar(CP_UTF8, 0, text, textLen, self->text, textLen);
    textLen = 2 * (wcslen(self->text) + 1);
    self->text = realloc(self->text, textLen);
}

static int B_MenuItem_create(MenuItem *self)
{
    B_MenuItem *bi = malloc(sizeof(B_MenuItem));
    bi->bo.t = BT_MenuItem;
    defaultBackend->privateApi->setBackendObject(self, bi);
    bi->m = self;
    bi->text = 0;
    B_MenuItem_updateText(bi);
    return 1;
}

static void B_MenuItem_destroy(MenuItem *self)
{
    if (!self) return;
    B_MenuItem *bi = defaultBackend->privateApi->backendObject(self);
    free(bi->text);
    free(bi);
}

static void wordKeyProvider(HashKey *key, const void *word)
{
    HashKey_set(key, sizeof(WORD), word);
}

static int B_Command_create(Command *self)
{
    B_Command *bc = malloc(sizeof(B_Command));
    bc->bo.t = BT_Command;
    defaultBackend->privateApi->setBackendObject(self, bc);
    bc->c = self;
    bc->id = ++bdata.nextCommandId;
    if (!bdata.commands)
    {
        bdata.commands = HashTable_create(
                HashTableSize_64, wordKeyProvider, 0, 0);
    }
    HashTable_set(bdata.commands, &bc->id, bc);
    return 1;
}

static void B_Command_destroy(Command *self)
{
    if (!self) return;
    B_Command *bc = defaultBackend->privateApi->backendObject(self);
    HashTable_remove(bdata.commands, &bc->id);
    free(bc);
}

static MessageBoxButton B_MessageBox_show(const Window *w, const char *title,
        const char *text, MessageBoxButton buttons, MessageBoxStyle style)
{
    LPWSTR titlew;
    LPWSTR textw;
    HWND hwnd = 0;

    MessageBoxButton b1 = buttons & ~MBB_Help;
    UINT type;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
    switch (b1)
    {
    case MBB_Cancel|MBB_Again|MBB_Continue:
        type = MB_CANCELTRYCONTINUE;
        break;
    case MBB_OK:
        type = MB_OK;
        break;
    case MBB_OK|MBB_Cancel:
        type = MB_OKCANCEL;
        break;
    case MBB_Retry|MBB_Cancel:
        type = MB_RETRYCANCEL;
        break;
    case MBB_Yes|MBB_No:
        type = MB_YESNO;
        break;
    case MBB_Yes|MBB_No|MBB_Cancel:
        type = MB_YESNOCANCEL;
        break;
    default:
        return MBB_Unsupported;
    }
#pragma GCC diagnostic pop

    if (buttons & MBB_Help) type |= MB_HELP;

    switch (style)
    {
    case MBS_Info:
        type |= MB_ICONINFORMATION;
        break;
    case MBS_Warning:
        type |= MB_ICONWARNING;
        break;
    case MBS_Error:
        type |= MB_ICONERROR;
        break;
    case MBS_Question:
        type |= MB_ICONQUESTION;
        break;
    case MBS_None:
    default:
        ;
    }

    size_t titlelen = strlen(title) + 1;
    size_t textlen = strlen(text) + 1;
    titlew = malloc(2 * titlelen);
    textw = malloc(2 * textlen);

    MultiByteToWideChar(CP_UTF8, 0, title, titlelen, titlew, titlelen);
    MultiByteToWideChar(CP_UTF8, 0, text, textlen, textw, textlen);

    if (w)
    {
        B_Window *bw = defaultBackend->privateApi->backendObject(w);
        hwnd = bw->hndl;
    }

    int ret = MessageBoxW(hwnd, textw, titlew, type);

    free(titlew);
    free(textw);

    switch (ret)
    {
    case IDABORT: return MBB_Unsupported;
    case IDCANCEL: return MBB_Cancel;
    case IDCONTINUE: return MBB_Continue;
    case IDIGNORE: return MBB_Unsupported;
    case IDNO: return MBB_No;
    case IDOK: return MBB_OK;
    case IDRETRY: return MBB_Retry;
    case IDTRYAGAIN: return MBB_Again;
    case IDYES: return MBB_Yes;
    default: return MBB_Unsupported;
    }
}

static void B_Label_updateText(Label *self, const char *text)
{
    B_Label *bl = defaultBackend->privateApi->backendObject(self);
    free(bl->text);
    if (text)
    {
        size_t textLen = strlen(text) + 1;
        bl->text = malloc(2*textLen);
        MultiByteToWideChar(CP_UTF8, 0, text, textLen, bl->text, textLen);
        textLen = 2 * wcslen(bl->text) + 2;
        bl->text = realloc(bl->text, textLen);
    }
    else
    {
        bl->text = 0;
    }
}

static int B_Label_create(Label *self)
{
    B_Label *bl = malloc(sizeof(B_Label));
    bl->bo.t = BT_Label;
    bl->l = self;
    bl->text = 0;
    defaultBackend->privateApi->setBackendObject(self, bl);
    const char *text = Label_text(self);
    B_Label_updateText(self, text);
    bl->hndl = INVALID_HANDLE_VALUE;
    return 1;
}

static void B_Label_setText(Label *self, const char *text)
{
    B_Label_updateText(self, text);
}

static void B_Label_destroy(Label *self)
{
    if (!self) return;
    B_Label *bl = defaultBackend->privateApi->backendObject(self);
    free(bl->text);
    free(bl);
}

static void setLabelContainer(B_Label *bl, void *container)
{
    BO *cbo = defaultBackend->privateApi->backendObject(container);
    HWND parent = INVALID_HANDLE_VALUE;
    if (cbo->t == BT_Window)
    {
        B_Window *bw = (B_Window *)cbo;
        parent = bw->hndl;
    }
    if (bl->hndl != INVALID_HANDLE_VALUE)
    {
        DestroyWindow(bl->hndl);
        bl->hndl = INVALID_HANDLE_VALUE;
    }
    if (parent != INVALID_HANDLE_VALUE)
    {
        bl->hndl = CreateWindowExW(0, L"Static", L"",
                WS_CHILD|WS_VISIBLE|SS_CENTER|SS_CENTERIMAGE,
                2, 2, 400, 80, parent, 0, GetModuleHandleW(0), 0);
        SetWindowTextW(bl->hndl, bl->text);
    }
}

static void B_Control_setContainer(void *self, void *container)
{
    BO *bo = defaultBackend->privateApi->backendObject(self);
    switch (bo->t)
    {
    case BT_Label:
        setLabelContainer((B_Label *)bo, container);
        break;
    }
}

static Backend win32Backend = {
    .backendApi = {
        .name = B_name,
        .control = {
            .setContainer = B_Control_setContainer,
        },
        .window = {
            .create = B_Window_create,
            .show = B_Window_show,
            .hide = B_Window_hide,
            .setMenu = B_Window_setMenu,
            .close = B_Window_close,
            .destroy = B_Window_destroy,
        },
        .menu = {
            .create = B_Menu_create,
            .addItem = B_Menu_addItem,
            .destroy = B_Menu_destroy,
        },
        .menuItem = {
            .create = B_MenuItem_create,
            .destroy = B_MenuItem_destroy,
        },
        .command = {
            .create = B_Command_create,
            .destroy = B_Command_destroy,
        },
        .messageBox = {
            .show = B_MessageBox_show,
        },
        .label = {
            .create = B_Label_create,
            .setText = B_Label_setText,
            .destroy = B_Label_destroy,
        },
    },
    .privateApi = 0,
};

SOLOCAL Backend *defaultBackend = &win32Backend;
