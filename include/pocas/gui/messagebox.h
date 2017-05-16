#ifndef POCAS_GUI_MESSAGEBOX_H
#define POCAS_GUI_MESSAGEBOX_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PG_Window);

enum PG_MessageBoxButton
{
    PG_MBB_Unsupported = 0,
    PG_MBB_Cancel = 1 << 0,
    PG_MBB_OK = 1 << 1,
    PG_MBB_Yes = 1 << 2,
    PG_MBB_No = 1 << 3,
    PG_MBB_Retry = 1 << 4,
    PG_MBB_Again = 1 << 5,
    PG_MBB_Continue = 1 << 6,
    PG_MBB_Help = 1 << 7
};
C_ENUM_DECL(PG_MessageBoxButton);

enum PG_MessageBoxStyle
{
    PG_MBS_None = 0,
    PG_MBS_Warning,
    PG_MBS_Info,
    PG_MBS_Error,
    PG_MBS_Question
};
C_ENUM_DECL(PG_MessageBoxStyle);

DECLEXPORT PG_MessageBoxButton PG_MessageBox_show(const PG_Window *w, const char *title,
        const char *text, PG_MessageBoxButton buttons, PG_MessageBoxStyle style);

#endif
