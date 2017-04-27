#include "c11threads.h"

#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>

#include <pocas/core/event_win32.h>
#include <pocas/core/eventloop_win32.h>
#include <pocas/core/hashtable.h>

#include <pocas/gui/command.h>
#include <pocas/gui/window.h>
#include <pocas/gui/menu.h>

#include "backend_internal.h"

struct B_Window
{
    Window *w;
    WNDCLASSEXW wc;
    HWND hndl;
    LPWSTR name;
};

struct B_Menu
{
    Menu *m;
    HMENU menu;
};

struct B_MenuItem
{
    MenuItem *m;
    LPWSTR text;
};

struct B_Command
{
    Command *c;
    WORD id;
};

static thread_local HashTable *commands = 0;
static thread_local WORD nextCommandId = 0x100;

static const char *B_name(void)
{
    return "win32";
}

static void handleWin32MessageEvent(void *w, EventArgs *args)
{
    B_Window *self = w;
    Win32MsgEvInfo *mei = EventArgs_evInfo(args);
    if (mei->wnd != self->hndl) return;

    WORD id;
    switch (mei->msg)
    {
    case WM_CLOSE:
        Window_close(self->w);
        EventArgs_setHandled(args);
        break;

    case WM_COMMAND:
        id = LOWORD(mei->wp);
        const B_Command *c = HashTable_get(commands, &id);
        if (c)
        {
            Command_invoke(c->c);
            EventArgs_setHandled(args);
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        EventArgs_setHandled(args);
        break;
    }
}

static B_Window *B_Window_create(Window *w)
{
    B_Window *self = calloc(1, sizeof(B_Window));
    const char *title = Window_title(w);
    size_t titleLen = strlen(title) + 1;
    self->name = calloc(1, 2 * titleLen);
    MultiByteToWideChar(CP_UTF8, 0, title, titleLen, self->name, titleLen);
    titleLen = 2 * (wcslen(self->name) + 1);
    self->name = realloc(self->name, titleLen);
    self->w = w;
    self->wc.cbSize = sizeof(WNDCLASSEXW);
    self->wc.hInstance = GetModuleHandleW(0);
    self->wc.lpszClassName = self->name;
    self->wc.lpfnWndProc = Eventloop_win32WndProc;
    self->wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
    self->wc.hCursor = LoadCursorA(0, IDC_ARROW);
    RegisterClassExW(&self->wc);
    self->hndl = CreateWindowExW(0, self->name, self->name,
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
            Window_width(w), Window_height(w), 0, 0,
            self->wc.hInstance, 0);
    Event_register(Eventloop_win32MsgEvent(), self, handleWin32MessageEvent);
    return self;
}

static void B_Window_show(B_Window *self)
{
    ShowWindow(self->hndl, SW_SHOWNORMAL);
    UpdateWindow(self->hndl);
}

static void B_Window_hide(B_Window *self)
{
    ShowWindow(self->hndl, SW_HIDE);
}

static void B_Window_setMenu(B_Window *self, B_Menu *menu)
{
    SetMenu(self->hndl, menu->menu);
    DrawMenuBar(self->hndl);
}

static void B_Window_close(B_Window *self)
{
    DestroyWindow(self->hndl);
}

static void B_Window_destroy(B_Window *self)
{
    if (!self) return;
    DestroyWindow(self->hndl);
    UnregisterClassW(self->name, self->wc.hInstance);
    Event_unregister(Eventloop_win32MsgEvent(), self, handleWin32MessageEvent);
    free(self->name);
    free(self);
}

static B_Menu *B_Menu_create(Menu *m)
{
    B_Menu *self = malloc(sizeof(B_Menu));
    self->m = m;
    self->menu = CreateMenu();
    return self;
}

static void B_Menu_addItem(B_Menu *self, B_MenuItem *item)
{
    Menu *subMenu = MenuItem_subMenu(item->m);
    if (subMenu)
    {
        B_Menu *sub = ((Frontend *)subMenu)->b;
        AppendMenuW(self->menu, MF_POPUP, (UINT_PTR)sub->menu, item->text);
    }
    else
    {
        Command *cmd = MenuItem_command(item->m);
        if (cmd)
        {
            B_Command *c = ((Frontend *)cmd)->b;
            AppendMenuW(self->menu, MF_STRING, (UINT_PTR)c->id, item->text);
        }
        else
        {
            AppendMenuW(self->menu, MF_STRING, 0, item->text);
        }
    }
}

static void B_Menu_destroy(B_Menu *self)
{
    if (!self) return;
    DestroyMenu(self->menu);
    free(self);
    return;
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

static B_MenuItem *B_MenuItem_create(MenuItem *m)
{
    B_MenuItem *self = malloc(sizeof(B_MenuItem));
    self->m = m;
    self->text = 0;
    B_MenuItem_updateText(self);
    return self;
}

static void B_MenuItem_destroy(B_MenuItem *self)
{
    if (!self) return;
    free(self->text);
    free(self);
    return;
}

static void wordKeyProvider(HashKey *key, const void *word)
{
    HashKey_set(key, sizeof(WORD), word);
}

static B_Command *B_Command_create(Command *c)
{
    B_Command *self = malloc(sizeof(B_Command));
    self->c = c;
    self->id = ++nextCommandId;
    if (!commands) commands = HashTable_create(HashTableSize_64, wordKeyProvider, 0, 0);
    HashTable_set(commands, &self->id, self);
    return self;
}

static void B_Command_destroy(B_Command *self)
{
    if (!self) return;
    HashTable_remove(commands, &self->id);
    free(self);
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
        B_Window *bw = ((const Frontend *)w)->b;
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

static Backend win32Backend = {
    .name = B_name,

    .Window_create = B_Window_create,
    .Window_show = B_Window_show,
    .Window_hide = B_Window_hide,
    .Window_setMenu = B_Window_setMenu,
    .Window_close = B_Window_close,
    .Window_destroy = B_Window_destroy,

    .Menu_create = B_Menu_create,
    .Menu_addItem = B_Menu_addItem,
    .Menu_destroy = B_Menu_destroy,

    .MenuItem_create = B_MenuItem_create,
    .MenuItem_destroy = B_MenuItem_destroy,

    .Command_create = B_Command_create,
    .Command_destroy = B_Command_destroy,

    .MessageBox_show = B_MessageBox_show
};

SOLOCAL const Backend *defaultBackend = &win32Backend;
