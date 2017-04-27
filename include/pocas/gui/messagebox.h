#ifndef POCAS_GUI_MESSAGEBOX_H
#define POCAS_GUI_MESSAGEBOX_H

#include <pocas/gui/decl.h>

typedef struct Window Window;

typedef enum MessageBoxButton
{
    MBB_Unsupported = 0,
    MBB_Cancel = 1 << 0,
    MBB_OK = 1 << 1,
    MBB_Yes = 1 << 2,
    MBB_No = 1 << 3,
    MBB_Retry = 1 << 4,
    MBB_Again = 1 << 5,
    MBB_Continue = 1 << 6,
    MBB_Help = 1 << 7
} MessageBoxButton;

DECLEXPORT MessageBoxButton MessageBox_show(const Window *w, const char *title,
        const char *text, MessageBoxButton buttons);

#endif
