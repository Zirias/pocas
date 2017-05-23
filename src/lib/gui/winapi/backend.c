#define _WIN32_WINNT 0x0600
#include "../c11threads.h"

#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <windows.h>
#include <commctrl.h>

#include <pocas/core/event_win32.h>
#include <pocas/core/eventloop_win32.h>
#include <pocas/core/hashtable.h>

#include <pocas/gui/private/backend.h>
#include <pocas/gui/backend/winapi.h>

struct bdata
{
    PC_HashTable *controls;
    WORD nextId;
    size_t nWindows;
    HWND activeWindow;
    int ncmInitialized;
    OSVERSIONINFOW vi;
    NONCLIENTMETRICSW ncm;
    HFONT messageFont;
    int messageFontHeight;
    int textControlHeight;
    int buttonWidth;
};

#define textBoxWidth 100

enum B_Type
{
    BT_Window,
    BT_Menu,
    BT_MenuItem,
    BT_Command,
    BT_Label,
    BT_Button,
    BT_TextBox
};

typedef struct BO
{
    enum B_Type t;
    HWND w;
} BO;

typedef struct BOTXT
{
    BO bo;
    LPWSTR text;
} BOTXT;

typedef struct B_Window
{
    BO bo;
    PG_Window *w;
    WNDCLASSEXW wc;
    LPWSTR name;
} B_Window;

typedef struct B_Menu
{
    BO bo;
    PG_Menu *m;
    HMENU menu;
} B_Menu;

typedef struct B_MenuItem
{
    BOTXT botxt;
    PG_MenuItem *m;
    WORD id;
} B_MenuItem;

typedef struct B_Label
{
    BOTXT botxt;
    PG_Label *l;
} B_Label;

typedef struct B_Button
{
    BOTXT botxt;
    PG_Button *b;
    WORD id;
} B_Button;

typedef struct B_TextBox
{
    BOTXT botxt;
    PG_TextBox *t;
    WORD id;
    PG_TextBox_textChanged changed;
    WNDPROC baseWndProc;
    int vshrink;
} B_TextBox;

static thread_local struct bdata bdata = {
    .controls = 0,
    .nextId = 0x100,
    .nWindows = 0,
    .ncmInitialized = 0,
    .activeWindow = INVALID_HANDLE_VALUE,
};

static const char *B_name(void)
{
    return "win32";
}

SOLOCAL PG_Backend *PG_winapiBackend;

/* hack to enable visual styles without relying on manifest
 * found at http://stackoverflow.com/a/10444161
 * modified for unicode-only code */
static int enableVisualStyles(void)
{
    wchar_t dir[MAX_PATH];
    ULONG_PTR ulpActivationCookie = 0;
    ACTCTXW actCtx =
    {
        sizeof(actCtx),
        ACTCTX_FLAG_RESOURCE_NAME_VALID
            | ACTCTX_FLAG_SET_PROCESS_DEFAULT
            | ACTCTX_FLAG_ASSEMBLY_DIRECTORY_VALID,
        L"shell32.dll", 0, 0, dir, (LPWSTR)124,
        0, 0
    };
    UINT cch = GetSystemDirectoryW(dir, sizeof(dir) / sizeof(*dir));
    if (cch >= sizeof(dir) / sizeof(*dir)) { return 0; }
    dir[cch] = L'\0';
    ActivateActCtx(CreateActCtxW(&actCtx), &ulpActivationCookie);
    INITCOMMONCONTROLSEX icx;
    icx.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icx.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icx);
    return (int) ulpActivationCookie;
}

static void wordKeyProvider(PC_HashKey *key, const void *word)
{
    PC_HashKey_set(key, sizeof(WORD), word);
}

static WORD registerControl(BO *bo)
{
    WORD id = ++bdata.nextId;
    if (!bdata.controls)
    {
        bdata.controls = PC_HashTable_create(
                PC_HTS_64, wordKeyProvider, 0, 0);
    }
    PC_HashTable_set(bdata.controls, &id, bo);
    return id;
}

static void unregisterControl(WORD id)
{
    PC_HashTable_remove(bdata.controls, &id);
}

static void initNcm(void)
{
    if (!bdata.ncmInitialized)
    {
        enableVisualStyles();
        bdata.vi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
        GetVersionExW(&bdata.vi);
        size_t ncmSize = sizeof(NONCLIENTMETRICSW);
        if (bdata.vi.dwMajorVersion < 6)
        {
            ncmSize -= 4;
            //bdata.ncm.iPaddedBorderWidth = 0;
        }
        bdata.ncm.cbSize = ncmSize;
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS,
                ncmSize, &bdata.ncm, 0);
        bdata.messageFont = CreateFontIndirectW(&bdata.ncm.lfMessageFont);
        HDC dc = GetDC(0);
        HGDIOBJ oldFont = SelectObject(dc, (HGDIOBJ) bdata.messageFont);
        SIZE sampleSize;
        GetTextExtentExPointA(dc,
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
                 52, 0, 0, 0, &sampleSize);
        SelectObject(dc, oldFont);
        ReleaseDC(0, dc);
        bdata.messageFontHeight = sampleSize.cy;
        bdata.buttonWidth = MulDiv(sampleSize.cx, 50, 4 * 52);
        bdata.textControlHeight = MulDiv(sampleSize.cy, 14, 8);
        bdata.ncmInitialized = 1;
    }
}

static void updateWindowClientSize(B_Window *self)
{
    RECT r;
    PG_Bounds b;

    GetClientRect(self->bo.w, &r);
    b.x = (unsigned int) r.left;
    b.y = (unsigned int) r.top;
    b.width = (unsigned int) (r.right - r.left);
    b.height = (unsigned int) (r.bottom - r.top);

    const PG_PrivateApi *api = PG_winapiBackend->privateApi;
    if (api->container.setBounds(self->w, &b))
    {
        InvalidateRect(self->bo.w, &r, 0);
    }
}

static void handleWin32MessageEvent(void *w, PC_EventArgs *args)
{
    B_Window *self = w;
    if (bdata.activeWindow != self->bo.w) return;

    MSG *msg = args->evInfo;
    if (IsDialogMessageW(self->bo.w, msg))
    {
        args->handled = 1;
    }
}

static LRESULT CALLBACK windowProc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp)
{
    B_Window *self = (B_Window *)GetPropW(wnd, L"B_Window");
    const PG_PrivateApi *api = PG_winapiBackend->privateApi;
    WORD id;
    HWND ctl;

    if (!self) return DefWindowProcW(wnd, msg, wp, lp);

    switch (msg)
    {
    case WM_SIZE:
        updateWindowClientSize(self);
        return 1;

    case WM_CLOSE:
        api->window.close(self->w);
        return 1;

    case WM_ACTIVATE:
        if (wp == 0) bdata.activeWindow = INVALID_HANDLE_VALUE;
        else bdata.activeWindow = wnd;
        break;

    case DM_GETDEFID:
        ctl = GetTopWindow(wnd);
        while (ctl)
        {
            wchar_t classname[16];
            GetClassNameW(ctl, classname, 16);
            if (!wcscmp(classname, L"Button"))
            {
                DWORD styles = GetWindowLongW(ctl, GWL_STYLE);
                if (styles & BS_DEFPUSHBUTTON) break;
            }
            ctl = GetWindow(ctl, GW_HWNDNEXT);
        }
        if (ctl)
        {
            id = (WORD) GetWindowLongW(ctl, GWL_ID);
            return MAKELONG(id, DC_HASDEFID);
        }
        return 0;

    case WM_COMMAND:
        id = LOWORD(wp);
        const BO *bo = PC_HashTable_get(bdata.controls, &id);
        if (bo)
        {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
            switch (bo->t)
            {
            case BT_Button:
                api->button.click(((B_Button *)bo)->b);
                return 1;
            case BT_MenuItem:
                api->menuItem.select(((B_MenuItem *)bo)->m);
                return 1;
            case BT_TextBox:
                if (HIWORD(wp) == EN_CHANGE)
                {
                    B_TextBox *bt = (B_TextBox *)bo;
                    free(bt->botxt.text);
                    int textsize = GetWindowTextLengthW(bo->w) + 1;
                    bt->botxt.text = calloc(textsize, sizeof(wchar_t));
                    GetWindowTextW(bo->w, bt->botxt.text, textsize);
                    char *converted = malloc(2 * textsize);
                    WideCharToMultiByte(CP_UTF8, 0, bt->botxt.text, -1, converted, 2 * textsize, 0, 0);
                    bt->changed(bt->t, converted);
                    free(converted);
                    return 1;
                }
                break;
            }
#pragma GCC diagnostic pop
        }
        break;

    case WM_DESTROY:
        PC_EventLoop_setProcessMessages(--bdata.nWindows);
        if (!bdata.nWindows)
        {
            PC_Event *lwc = api->window.lastWindowClosedEvent();
            PC_EventArgs lwcArgs = PC_EventArgs_init(lwc, 0, 0);
            PC_Event_raise(lwc, &lwcArgs);
            if (!lwcArgs.handled)
            {
                PostQuitMessage(0);
            }
        }
        return 1;
    }
    return DefWindowProcW(wnd, msg, wp, lp);
}

static int B_Window_create(PG_Window *self)
{
    const PG_PrivateApi *api = PG_winapiBackend->privateApi;
    initNcm();
    B_Window *bw = calloc(1, sizeof(B_Window));
    PG_winapiBackend->privateApi->setBackendObject(self, bw);
    bw->bo.t = BT_Window;
    const char *title = api->window.title(self);
    size_t titleLen = strlen(title) + 1;
    bw->name = calloc(1, 2 * titleLen);
    MultiByteToWideChar(CP_UTF8, 0, title, titleLen, bw->name, titleLen);
    titleLen = 2 * (wcslen(bw->name) + 1);
    bw->name = realloc(bw->name, titleLen);
    bw->w = self;
    PG_Window *parent = api->window.parent(self);
    HWND pw = 0;
    if (parent)
    {
        B_Window *bpw = PG_winapiBackend->privateApi->backendObject(parent);
        pw = bpw->bo.w;
    }
    bw->wc.cbSize = sizeof(WNDCLASSEXW);
    bw->wc.hInstance = GetModuleHandleW(0);
    bw->wc.lpszClassName = bw->name;
    bw->wc.lpfnWndProc = windowProc;
    bw->wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
    bw->wc.hCursor = LoadCursorA(0, IDC_ARROW);
    RegisterClassExW(&bw->wc);
    RECT winrect;
    memset(&winrect, 0, sizeof(RECT));
    winrect.right = api->window.width(self);
    winrect.bottom = api->window.height(self);
    AdjustWindowRect(&winrect, WS_OVERLAPPEDWINDOW, 0);
    bw->bo.w = CreateWindowExW(0, bw->name, bw->name,
            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
            winrect.right - winrect.left, winrect.bottom - winrect.top,
            pw, 0, bw->wc.hInstance, 0);
    SetPropW(bw->bo.w, L"B_Window", (HANDLE)bw);
    PC_Event_register(PC_EventLoop_win32MsgEvent(), bw, handleWin32MessageEvent);
    PC_EventLoop_setProcessMessages(++bdata.nWindows);
    return 1;
}

static void B_Window_setMenu(PG_Window *self, PG_Menu *menu)
{
    B_Window *bw = PG_winapiBackend->privateApi->backendObject(self);
    B_Menu *bm = PG_winapiBackend->privateApi->backendObject(menu);
    SetMenu(bw->bo.w, bm->menu);
    DrawMenuBar(bw->bo.w);
}

static void B_Window_close(PG_Window *self)
{
    B_Window *bw = PG_winapiBackend->privateApi->backendObject(self);
    DestroyWindow(bw->bo.w);
}

static void B_Window_destroy(PG_Window *self)
{
    if (!self) return;
    B_Window *bw = PG_winapiBackend->privateApi->backendObject(self);
    DestroyWindow(bw->bo.w);
    UnregisterClassW(bw->name, bw->wc.hInstance);
    PC_Event_unregister(PC_EventLoop_win32MsgEvent(), bw, handleWin32MessageEvent);
    free(bw->name);
    free(bw);
}

static PG_MessageBoxButton B_MessageBox_show(const PG_Window *w, const char *title,
        const char *text, PG_MessageBoxButton buttons, PG_MessageBoxStyle style)
{
    LPWSTR titlew;
    LPWSTR textw;
    HWND hwnd = 0;

    PG_MessageBoxButton b1 = buttons & ~PG_MBB_Help;
    UINT type;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
    switch (b1)
    {
    case PG_MBB_Cancel|PG_MBB_Again|PG_MBB_Continue:
        type = MB_CANCELTRYCONTINUE;
        break;
    case PG_MBB_OK:
        type = MB_OK;
        break;
    case PG_MBB_OK|PG_MBB_Cancel:
        type = MB_OKCANCEL;
        break;
    case PG_MBB_Retry|PG_MBB_Cancel:
        type = MB_RETRYCANCEL;
        break;
    case PG_MBB_Yes|PG_MBB_No:
        type = MB_YESNO;
        break;
    case PG_MBB_Yes|PG_MBB_No|PG_MBB_Cancel:
        type = MB_YESNOCANCEL;
        break;
    default:
        return PG_MBB_Unsupported;
    }
#pragma GCC diagnostic pop

    if (buttons & PG_MBB_Help) type |= MB_HELP;

    switch (style)
    {
    case PG_MBS_Info:
        type |= MB_ICONINFORMATION;
        break;
    case PG_MBS_Warning:
        type |= MB_ICONWARNING;
        break;
    case PG_MBS_Error:
        type |= MB_ICONERROR;
        break;
    case PG_MBS_Question:
        type |= MB_ICONQUESTION;
        break;
    case PG_MBS_None:
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
        B_Window *bw = PG_winapiBackend->privateApi->backendObject(w);
        hwnd = bw->bo.w;
    }

    int ret = MessageBoxW(hwnd, textw, titlew, type);

    free(titlew);
    free(textw);

    switch (ret)
    {
    case IDABORT: return PG_MBB_Unsupported;
    case IDCANCEL: return PG_MBB_Cancel;
    case IDCONTINUE: return PG_MBB_Continue;
    case IDIGNORE: return PG_MBB_Unsupported;
    case IDNO: return PG_MBB_No;
    case IDOK: return PG_MBB_OK;
    case IDRETRY: return PG_MBB_Retry;
    case IDTRYAGAIN: return PG_MBB_Again;
    case IDYES: return PG_MBB_Yes;
    default: return PG_MBB_Unsupported;
    }
}

static void BOTXT_measure(void *self)
{
    BOTXT *bt = PG_winapiBackend->privateApi->backendObject(self);
    PG_Size size;
    if (bt->bo.t == BT_Button)
    {
        size.width = bdata.buttonWidth;
        size.height = bdata.textControlHeight;
    }
    else if (bt->text)
    {
        SIZE waSize;
        HDC dc = GetDC(bt->bo.w);
        HGDIOBJ oldFont = SelectObject(dc, (HGDIOBJ) bdata.messageFont);
        GetTextExtentExPointW(dc, bt->text, wcslen(bt->text), 0, 0, 0, &waSize);
        SelectObject(dc, oldFont);
        ReleaseDC(bt->bo.w, dc);
        size.width = (int) waSize.cx;
        size.height = (int) waSize.cy;
    }
    else
    {
        size.width = 0;
        size.height = 0;
    }
    PG_winapiBackend->privateApi->control.setContentSize(self, &size);
}

static void BOTXT_updateText(void *self, const char *text)
{
    BOTXT *bt = PG_winapiBackend->privateApi->backendObject(self);
    free(bt->text);
    if (text)
    {
        size_t textLen = strlen(text) + 1;
        bt->text = malloc(2*textLen);
        MultiByteToWideChar(CP_UTF8, 0, text, textLen, bt->text, textLen);
        textLen = 2 * wcslen(bt->text) + 2;
        bt->text = realloc(bt->text, textLen);
    }
    else
    {
        bt->text = 0;
    }
    if (bt->bo.w != INVALID_HANDLE_VALUE)
    {
        if (bt->bo.t == BT_Label)
        {
            BOTXT_measure(self);
        }
        SetWindowTextW(bt->bo.w, bt->text);
    }
}

static int B_Menu_create(PG_Menu *self)
{
    B_Menu *bm = malloc(sizeof(B_Menu));
    bm->bo.t = BT_Menu;
    bm->bo.w = INVALID_HANDLE_VALUE;
    PG_winapiBackend->privateApi->setBackendObject(self, bm);
    bm->m = self;
    bm->menu = CreateMenu();
    return 1;
}

static void B_Menu_addItem(PG_Menu *self, PG_MenuItem *item)
{
    const PG_PrivateApi *api = PG_winapiBackend->privateApi;
    B_Menu *bm = api->backendObject(self);
    B_MenuItem *bi = api->backendObject(item);
    PG_Menu *subMenu = api->menuItem.subMenu(item);
    if (subMenu)
    {
        B_Menu *sub = api->backendObject(subMenu);
        AppendMenuW(bm->menu, MF_POPUP, (UINT_PTR)sub->menu, bi->botxt.text);
    }
    else
    {
        bi->id = registerControl((BO *)bi);
        AppendMenuW(bm->menu, MF_STRING, (UINT_PTR)bi->id, bi->botxt.text);
    }
}

static void B_Menu_destroy(PG_Menu *self)
{
    if (!self) return;
    B_Menu *bm = PG_winapiBackend->privateApi->backendObject(self);
    DestroyMenu(bm->menu);
    free(bm);
}

static int B_MenuItem_create(PG_MenuItem *self)
{
    const PG_PrivateApi *api = PG_winapiBackend->privateApi;
    B_MenuItem *bi = malloc(sizeof(B_MenuItem));
    bi->botxt.bo.t = BT_MenuItem;
    bi->botxt.bo.w = INVALID_HANDLE_VALUE;
    api->setBackendObject(self, bi);
    bi->m = self;
    bi->botxt.text = 0;
    const char *text = api->menuItem.text(self);
    BOTXT_updateText(self, text);
    return 1;
}

static void B_MenuItem_destroy(PG_MenuItem *self)
{
    if (!self) return;
    B_MenuItem *bi = PG_winapiBackend->privateApi->backendObject(self);
    free(bi->botxt.text);
    free(bi);
}

static int B_Label_create(PG_Label *self)
{
    const PG_PrivateApi *api = PG_winapiBackend->privateApi;
    B_Label *bl = malloc(sizeof(B_Label));
    bl->botxt.bo.t = BT_Label;
    bl->botxt.bo.w = INVALID_HANDLE_VALUE;
    bl->l = self;
    bl->botxt.text = 0;
    api->setBackendObject(self, bl);
    const char *text = api->label.text(self);
    BOTXT_updateText(self, text);
    return 1;
}

static void B_Label_setText(PG_Label *self, const char *text)
{
    BOTXT_updateText(self, text);
}

static void B_Label_destroy(PG_Label *self)
{
    if (!self) return;
    B_Label *bl = PG_winapiBackend->privateApi->backendObject(self);
    free(bl->botxt.text);
    free(bl);
}

static int B_Button_create(PG_Button *self)
{
    const PG_PrivateApi *api = PG_winapiBackend->privateApi;
    B_Button *bb = malloc(sizeof(B_Button));
    bb->botxt.bo.t = BT_Button;
    bb->botxt.bo.w = INVALID_HANDLE_VALUE;
    bb->b = self;
    bb->botxt.text = 0;
    api->setBackendObject(self, bb);
    const char *text = api->button.text(self);
    BOTXT_updateText(self, text);
    bb->id = registerControl((BO *)bb);
    return 1;
}

static void B_Button_setText(PG_Button *self, const char *text)
{
    BOTXT_updateText(self, text);
}

static void B_Button_destroy(PG_Button *self)
{
    if (!self) return;
    B_Button *bb = PG_winapiBackend->privateApi->backendObject(self);
    unregisterControl(bb->id);
    free(bb->botxt.text);
    free(bb);
}

static int B_TextBox_create(PG_TextBox *self, PG_TextBox_textChanged changed)
{
    B_TextBox *bt = calloc(1, sizeof(B_TextBox));
    bt->botxt.bo.t = BT_TextBox;
    bt->botxt.bo.w = INVALID_HANDLE_VALUE;
    bt->t = self;
    bt->changed = changed;
    PG_winapiBackend->privateApi->setBackendObject(self, bt);
    bt->id = registerControl((BO *)bt);
    PG_Size size = PG_Size_init(textBoxWidth, bdata.textControlHeight);
    PG_winapiBackend->privateApi->control.setContentSize(self, &size);
    return 1;
}

static void B_TextBox_setText(PG_TextBox *self, const char *text)
{
    BOTXT_updateText(self, text);
}

static void B_TextBox_destroy(PG_TextBox *self)
{
    if (!self) return;
    B_TextBox *bt = PG_winapiBackend->privateApi->backendObject(self);
    unregisterControl(bt->id);
    free(bt->botxt.text);
    free(bt);
}

static void B_Control_setBounds(void *self, const PG_Bounds *b)
{
    BO *bo = PG_winapiBackend->privateApi->backendObject(self);
    if (!bo) return;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
    switch (bo->t)
    {
    case BT_Label:
    case BT_Button:
    case BT_TextBox:
        if (bo->w != INVALID_HANDLE_VALUE)
        {
            MoveWindow(bo->w, b->x, b->y, b->width, b->height, 1);
        }
        break;
    }
#pragma GCC diagnostic pop
}

static HWND findParentControlWindow(void *control)
{
    void *container = PG_winapiBackend->privateApi->control.container(control);
    if (!container) return INVALID_HANDLE_VALUE;
    BO *cbo = PG_winapiBackend->privateApi->backendObject(container);
    while (!cbo || cbo->w == INVALID_HANDLE_VALUE)
    {
        void *cc = PG_winapiBackend->privateApi->controlObject(container);
        if (!cc) return INVALID_HANDLE_VALUE;
        container = PG_winapiBackend->privateApi->control.container(container);
        if (!container) return INVALID_HANDLE_VALUE;
        cbo = PG_winapiBackend->privateApi->backendObject(container);
    }
    return cbo->w;
}

static LRESULT CALLBACK textBoxProc(HWND w, UINT msg, WPARAM wp, LPARAM lp)
{
    B_TextBox *bt = (B_TextBox *)GetPropW(w, L"B_TextBox");
    WNDCLASSEXW wc;

    switch (msg)
    {
    case WM_ERASEBKGND:
        wc.cbSize = sizeof(wc);
        GetClassInfoExW(0, L"Edit", &wc);
        RECT cr;
        GetClientRect(w, &cr);
        cr.top -= bt->vshrink;
        cr.bottom += bt->vshrink;
        HDC dc = GetDC(w);
        FillRect(dc, &cr, wc.hbrBackground);
        ReleaseDC(w, dc);
        return 1;

    case WM_NCCALCSIZE:
        if (!wp) break;
        LRESULT result = CallWindowProcW(bt->baseWndProc, w, msg, wp, lp);
        NCCALCSIZE_PARAMS *p = (NCCALCSIZE_PARAMS *)lp;
        int height = p->rgrc[0].bottom - p->rgrc[0].top;
        bt->vshrink = 0;
        if (height > bdata.messageFontHeight + 3)
        {
            bt->vshrink = (height - bdata.messageFontHeight - 3) / 2;
            p->rgrc[0].top += bt->vshrink;
            p->rgrc[0].bottom -= bt->vshrink;
        }
        return result;
    }

    return CallWindowProcW(bt->baseWndProc, w, msg, wp, lp);
}

static int createChildControlWindow(void *control,
        const wchar_t *wc, DWORD style, DWORD exStyle, HMENU id)
{
    const PG_PrivateApi *api = PG_winapiBackend->privateApi;
    BO *bo = api->backendObject(control);
    PG_Bounds b;
    api->control.bounds(control, &b);
    HWND parent = findParentControlWindow(control);
    if (parent == INVALID_HANDLE_VALUE)
    {
        bo->w = INVALID_HANDLE_VALUE;
        return 0;
    }
    style |= WS_CHILD;
    if (api->control.shown(control)) style |= WS_VISIBLE;
    bo->w = CreateWindowExW(exStyle, wc, L"", style, b.x, b.y, b.width, b.height,
            parent, id, GetModuleHandleW(0), 0);
    if (bo->t == BT_TextBox)
    {
        B_TextBox *bt = (B_TextBox *)bo;
        bt->baseWndProc = (WNDPROC)SetWindowLongPtrW(bo->w, GWLP_WNDPROC,
                (LONG_PTR)textBoxProc);
        SetPropW(bo->w, L"B_TextBox", (HANDLE)bt);
        SetWindowPos(bo->w, 0, 0, 0, 0, 0,
                SWP_NOOWNERZORDER|SWP_NOSIZE|SWP_NOMOVE|SWP_FRAMECHANGED);
    }
    return 1;
}

static int createTextControlWindow(void *control, HMENU id)
{
    wchar_t *wc = 0;
    DWORD style = 0;
    DWORD exStyle = 0;

    const PG_PrivateApi *api = PG_winapiBackend->privateApi;
    BOTXT *bt = api->backendObject(control);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
    switch (bt->bo.t)
    {
    case BT_Label:
        wc = L"Static";
        PG_LabelStyle ls = api->label.style(control);
        if (ls & PG_LS_AlignRight)
        {
            style |= SS_RIGHT;
        }
        if (ls & PG_LS_HCenter)
        {
            style |= SS_CENTER;
        }
        if (ls & PG_LS_VCenter)
        {
            style |= SS_CENTERIMAGE;
        }
        if (!style) style = SS_SIMPLE;
        break;
    case BT_Button:
        wc = L"Button";
        style = BS_TEXT|WS_TABSTOP|WS_GROUP;
        switch (api->button.style(control))
        {
        case PG_BS_Normal:
            style |= BS_PUSHBUTTON;
            break;
        case PG_BS_Default:
            style |= BS_DEFPUSHBUTTON;
            break;
        }
        break;
    case BT_TextBox:
        wc = L"Edit";
        style = ES_AUTOHSCROLL|WS_TABSTOP|WS_GROUP;
        exStyle = WS_EX_CLIENTEDGE;
        break;
    default:
        return 0;
    }
#pragma GCC diagnostic pop

    if (createChildControlWindow(control, wc, style, exStyle, id))
    {
        initNcm();
        if (bt->bo.t == BT_Label || bt->bo.t == BT_Button)
        {
            BOTXT_measure(control);
        }
        SendMessageW(bt->bo.w, WM_SETFONT, (WPARAM) bdata.messageFont, 1);
        if (bt->text)
        {
            SetWindowTextW(bt->bo.w, bt->text);
        }
        return 1;
    }
    return 0;
}

static void B_Control_setShown(void *self, int shown)
{
    BO *bo = PG_winapiBackend->privateApi->backendObject(self);
    if (!bo) return;
    HMENU id = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
    switch (bo->t)
    {
    case BT_Button:
        id = (HMENU)(UINT_PTR)((B_Button *)bo)->id;
        goto createTextControl;
    case BT_TextBox:
        id = (HMENU)(UINT_PTR)((B_TextBox *)bo)->id;
        goto createTextControl;
    case BT_Label:
        createTextControl:
        if (bo->w == INVALID_HANDLE_VALUE)
        {
            createTextControlWindow(self, id);
        }
        break;
    }
#pragma GCC diagnostic pop

    if (bo->w != INVALID_HANDLE_VALUE)
    {
        ShowWindow(bo->w, shown ? SW_NORMAL : SW_HIDE);
    }
}

static void B_Control_setEnabled(void *self, int enabled)
{
    BO *bo = PG_winapiBackend->privateApi->backendObject(self);
    if (!bo) return;
    if (bo->w != INVALID_HANDLE_VALUE)
    {
        EnableWindow(bo->w, !!enabled);
    }
}

static void B_Window_showModal(PG_Window *self)
{
    const PG_PrivateApi *api = PG_winapiBackend->privateApi;
    api->control.setShown(self, 1);
    B_Window *bw = api->backendObject(self);
    PG_Window *parentWindow = api->window.parent(bw->w);
    if (parentWindow)
    {
        B_Control_setEnabled(parentWindow, 0);
    }
}

static void B_Window_hideModal(PG_Window *self)
{
    const PG_PrivateApi *api = PG_winapiBackend->privateApi;
    B_Window *bw = api->backendObject(self);
    PG_Window *parentWindow = api->window.parent(bw->w);
    if (parentWindow)
    {
        B_Control_setEnabled(parentWindow, 1);
    }
    api->control.setShown(self, 0);
}

static void B_Control_focus(void *self)
{
    BO *bo = PG_winapiBackend->privateApi->backendObject(self);
    if (!bo) return;
    if (bo->w != INVALID_HANDLE_VALUE)
    {
        SetFocus(bo->w);
        if (bo->t == BT_TextBox)
        {
            SendMessageW(bo->w, EM_SETSEL, 0, -1);
        }
    }
}

static void setTextControlParent(void *self)
{
    BOTXT *bt = PG_winapiBackend->privateApi->backendObject(self);
    HMENU id = 0;
    if (bt->bo.t == BT_Button)
    {
        id = (HMENU)(UINT_PTR)((B_Button *)bt)->id;
    }
    else if (bt->bo.t == BT_TextBox)
    {
        id = (HMENU)(UINT_PTR)((B_TextBox *)bt)->id;
    }
    if (bt->bo.w == INVALID_HANDLE_VALUE)
    {
        createTextControlWindow(self, id);
    }
    else
    {
        HWND currentParent = GetParent(bt->bo.w);
        HWND newParent = findParentControlWindow(self);
        if (currentParent != newParent)
        {
            DestroyWindow(bt->bo.w);
            createTextControlWindow(self, id);
        }
    }
}

static void B_Control_setContainer(void *self, void *container)
{
    (void)container;
    BO *bo = PG_winapiBackend->privateApi->backendObject(self);
    if (!bo) return;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
    switch (bo->t)
    {
    case BT_Label:
    case BT_Button:
    case BT_TextBox:
        setTextControlParent(self);
        break;
    }
#pragma GCC diagnostic pop
}

static PG_Backend backend_winapi = {
    .backendApi = {
        .name = B_name,
        .control = {
            .setContainer = B_Control_setContainer,
            .setBounds = B_Control_setBounds,
            .setShown = B_Control_setShown,
            .setEnabled = B_Control_setEnabled,
            .focus = B_Control_focus,
        },
        .window = {
            .create = B_Window_create,
            .setMenu = B_Window_setMenu,
            .close = B_Window_close,
            .showModal = B_Window_showModal,
            .hideModal = B_Window_hideModal,
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
        .messageBox = {
            .show = B_MessageBox_show,
        },
        .label = {
            .create = B_Label_create,
            .setText = B_Label_setText,
            .destroy = B_Label_destroy,
        },
        .button = {
            .create = B_Button_create,
            .setText = B_Button_setText,
            .destroy = B_Button_destroy
        },
        .textBox = {
            .create = B_TextBox_create,
            .setText = B_TextBox_setText,
            .destroy = B_TextBox_destroy,
        },
    },
    .privateApi = 0,
};

SOEXPORT PG_Backend *PG_winapiBackend = &backend_winapi;
