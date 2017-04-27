#include <stdlib.h>
#include <windows.h>

#include <pocas/core/event_win32.h>
#include <pocas/core/eventloop_win32.h>

#include <pocas/gui/window.h>
#include "backend_internal.h"

struct B_Window
{
    Window *w;
    WNDCLASSEXW wc;
    HWND hndl;
    LPWSTR name;
};

static const char *B_name(void)
{
    return "win32";
}

static void handleWin32MessageEvent(void *w, EventArgs *args)
{
    B_Window *self = w;
    Win32MsgEvInfo *mei = EventArgs_evInfo(args);
    if (mei->wnd != self->hndl) return;

    switch (mei->msg)
    {
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
    self->w = w;
    self->wc.cbSize = sizeof(WNDCLASSEXW);
    self->wc.hInstance = GetModuleHandleW(0);
    self->wc.lpszClassName = self->name;
    self->wc.lpfnWndProc = Eventloop_win32WndProc;
    self->wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
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

static void B_Window_destroy(B_Window *self)
{
    if (!self) return;
    Event_unregister(Eventloop_win32MsgEvent(), self, handleWin32MessageEvent);
    DestroyWindow(self->hndl);
    UnregisterClassW(self->name, self->wc.hInstance);
    free(self->name);
    free(self);
}

static Backend win32Backend = {
    .name = B_name,
    .Window_create = B_Window_create,
    .Window_show = B_Window_show,
    .Window_hide = B_Window_hide,
    .Window_destroy = B_Window_destroy
};

SOLOCAL const Backend *defaultBackend = &win32Backend;
