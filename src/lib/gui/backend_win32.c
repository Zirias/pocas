#define _WIN32_WINNT 0x0600
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
#include <pocas/gui/control.h>
#include <pocas/gui/window.h>
#include <pocas/gui/menu.h>
#include <pocas/gui/label.h>

struct bdata
{
    HashTable *commands;
    WORD nextCommandId;
    size_t nWindows;
    int ncmInitialized;
    OSVERSIONINFOW vi;
    NONCLIENTMETRICSW ncm;
    HFONT messageFont;
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
    HWND w;
} BO;

typedef struct B_Window
{
    BO bo;
    Window *w;
    WNDCLASSEXW wc;
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
    LPWSTR text;
} B_Label;

static thread_local struct bdata bdata = {
    .commands = 0,
    .nextCommandId = 0x100,
    .nWindows = 0,
    .ncmInitialized = 0,
};

static const char *B_name(void)
{
    return "win32";
}

SOLOCAL Backend *defaultBackend;

static void initNcm(void)
{
    if (!bdata.ncmInitialized)
    {
        bdata.vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
        GetVersionExW(&bdata.vi);
        size_t ncmSize = sizeof(NONCLIENTMETRICSW);
        if (bdata.vi.dwMajorVersion < 6)
        {
            ncmSize -= 4;
            bdata.ncm.iPaddedBorderWidth = 0;
        }
        bdata.ncm.cbSize = ncmSize;
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS,
                ncmSize, &bdata.ncm, 0);
        bdata.messageFont = CreateFontIndirectW(&bdata.ncm.lfMessageFont);
        bdata.ncmInitialized = 1;
    }
}

static void updateWindowClientSize(B_Window *self)
{
    RECT r;
    Bounds b;

    GetClientRect(self->bo.w, &r);
    b.x = (unsigned int) r.left;
    b.y = (unsigned int) r.top;
    b.width = (unsigned int) (r.right - r.left);
    b.height = (unsigned int) (r.bottom - r.top);

    const GuiPrivateApi *api = defaultBackend->privateApi;
    if (api->container.setBounds(self->w, &b))
    {
        InvalidateRect(self->bo.w, &r, 0);
        UpdateWindow(self->bo.w);
    }
}

static void handleWin32MessageEvent(void *w, EventArgs *args)
{
    B_Window *self = w;
    Win32MsgEvInfo *mei = EventArgs_evInfo(args);
    if (mei->wnd != self->bo.w) return;

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
    initNcm();
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
    RECT winrect;
    memset(&winrect, 0, sizeof(RECT));
    winrect.right = Window_width(self);
    winrect.bottom = Window_height(self);
    AdjustWindowRect(&winrect, WS_OVERLAPPEDWINDOW, 0);
    bw->bo.w = CreateWindowExW(0, bw->name, bw->name,
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
            winrect.right - winrect.left, winrect.bottom - winrect.top,
            0, 0, bw->wc.hInstance, 0);
    Event_register(EventLoop_win32MsgEvent(), bw, handleWin32MessageEvent);
    EventLoop_setProcessMessages(++bdata.nWindows);
    return 1;
}

static void B_Window_setMenu(Window *self, Menu *menu)
{
    B_Window *bw = defaultBackend->privateApi->backendObject(self);
    B_Menu *bm = defaultBackend->privateApi->backendObject(menu);
    SetMenu(bw->bo.w, bm->menu);
    DrawMenuBar(bw->bo.w);
}

static void B_Window_close(Window *self)
{
    B_Window *bw = defaultBackend->privateApi->backendObject(self);
    DestroyWindow(bw->bo.w);
}

static void B_Window_destroy(Window *self)
{
    if (!self) return;
    B_Window *bw = defaultBackend->privateApi->backendObject(self);
    DestroyWindow(bw->bo.w);
    UnregisterClassW(bw->name, bw->wc.hInstance);
    Event_unregister(EventLoop_win32MsgEvent(), bw, handleWin32MessageEvent);
    free(bw->name);
    free(bw);
}

static int B_Menu_create(Menu *self)
{
    B_Menu *bm = malloc(sizeof(B_Menu));
    bm->bo.t = BT_Menu;
    bm->bo.w = INVALID_HANDLE_VALUE;
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
    bi->bo.w = INVALID_HANDLE_VALUE;
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
    bc->bo.w = INVALID_HANDLE_VALUE;
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
        hwnd = bw->bo.w;
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

static void measureLabelText(void *label)
{
    B_Label *bl = defaultBackend->privateApi->backendObject(label);
    SIZE textSize;
    HDC dc = GetDC(bl->bo.w);
    SelectObject(dc, (HGDIOBJ) bdata.messageFont);
    GetTextExtentExPointW(dc, bl->text, wcslen(bl->text), 0, 0, 0, &textSize);
    ReleaseDC(bl->bo.w, dc);
    defaultBackend->privateApi->control.setContentSize(label,
            textSize.cx, textSize.cy);
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
    if (bl->bo.w != INVALID_HANDLE_VALUE)
    {
        measureLabelText(self);
        SetWindowTextW(bl->bo.w, bl->text);
    }
}

static int B_Label_create(Label *self)
{
    B_Label *bl = malloc(sizeof(B_Label));
    bl->bo.t = BT_Label;
    bl->bo.w = INVALID_HANDLE_VALUE;
    bl->l = self;
    bl->text = 0;
    defaultBackend->privateApi->setBackendObject(self, bl);
    const char *text = Label_text(self);
    B_Label_updateText(self, text);
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

static void B_Control_setBounds(void *self, const Bounds *b)
{
    BO *bo = defaultBackend->privateApi->backendObject(self);
    if (!bo) return;
    switch (bo->t)
    {
    case BT_Label:
        if (bo->w != INVALID_HANDLE_VALUE)
        {
            MoveWindow(bo->w, b->x, b->y, b->width, b->height, 1);
        }
        break;
    }
}

static HWND findParentControlWindow(void *control)
{
    void *container = defaultBackend->privateApi->control.container(control);
    if (!container) return INVALID_HANDLE_VALUE;
    BO *cbo = defaultBackend->privateApi->backendObject(container);
    while (!cbo || cbo->w == INVALID_HANDLE_VALUE)
    {
        void *cc = defaultBackend->privateApi->controlObject(container);
        if (!cc) return INVALID_HANDLE_VALUE;
        container = defaultBackend->privateApi->control.container(cc);
        if (!container) return INVALID_HANDLE_VALUE;
        cbo = defaultBackend->privateApi->backendObject(container);
    }
    return cbo->w;
}

static int createChildControlWindow(void *control,
        const wchar_t *wc, DWORD style)
{
    BO *bo = defaultBackend->privateApi->backendObject(control);
    Bounds b;
    Control_bounds(control, &b);
    HWND parent = findParentControlWindow(control);
    if (parent == INVALID_HANDLE_VALUE)
    {
        bo->w = INVALID_HANDLE_VALUE;
        return 0;
    }
    style |= WS_CHILD;
    if (Control_shown(control)) style |= WS_VISIBLE;
    bo->w = CreateWindowExW(0, wc, L"", style, b.x, b.y, b.width, b.height,
            parent, 0, GetModuleHandleW(0), 0);
    return 1;
}

static int createLabelWindow(void *label)
{
    if (createChildControlWindow(label, L"Static", SS_CENTER|SS_CENTERIMAGE))
    {
        initNcm();
        B_Label *bl = defaultBackend->privateApi->backendObject(label);
        measureLabelText(label);
        SendMessageW(bl->bo.w, WM_SETFONT, (WPARAM) bdata.messageFont, 1);
        SetWindowTextW(bl->bo.w, bl->text);
    }
}

static void B_Control_setShown(void *self, int shown)
{
    BO *bo = defaultBackend->privateApi->backendObject(self);
    if (!bo) return;
    switch (bo->t)
    {
    case BT_Label:
        if (bo->w == INVALID_HANDLE_VALUE)
        {
            createLabelWindow(self);
        }
        break;
    }

    if (bo->w != INVALID_HANDLE_VALUE)
    {
        ShowWindow(bo->w, shown ? SW_NORMAL : SW_HIDE);
        UpdateWindow(bo->w);
    }
}

static void B_Control_setContainer(void *self, void *container)
{
    BO *bo = defaultBackend->privateApi->backendObject(self);
    if (!bo) return;
    switch (bo->t)
    {
    case BT_Label:
        if (bo->w == INVALID_HANDLE_VALUE)
        {
            createLabelWindow(self);
        }
        else
        {
            HWND currentParent = GetParent(bo->w);
            HWND newParent = findParentControlWindow(self);
            if (currentParent != newParent)
            {
                DestroyWindow(bo->w);
                createLabelWindow(self);
            }
        }
        break;
    }
}

static Backend win32Backend = {
    .backendApi = {
        .name = B_name,
        .control = {
            .setContainer = B_Control_setContainer,
            .setBounds = B_Control_setBounds,
            .setShown = B_Control_setShown,
        },
        .window = {
            .create = B_Window_create,
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
