#include "backend_internal.h"

#include <pocas/gui/messagebox.h>

SOEXPORT MessageBoxButton MessageBox_show(const Window *w, const char *title,
        const char *text, MessageBoxButton buttons, MessageBoxStyle style)
{
    const Backend *b = Backend_current();
    return b->MessageBox_show ? b->MessageBox_show(
            w, title, text, buttons, style) : MBB_Unsupported;
}
