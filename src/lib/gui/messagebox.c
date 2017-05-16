#include "internal.h"

#include <pocas/gui/messagebox.h>

SOEXPORT PG_MessageBoxButton PG_MessageBox_show(const PG_Window *w, const char *title,
        const char *text, PG_MessageBoxButton buttons, PG_MessageBoxStyle style)
{
    const PG_Backend *b = PG_Backend_current();
    return b->backendApi.messageBox.show ? b->backendApi.messageBox.show(
            w, title, text, buttons, style) : PG_MBB_Unsupported;
}
