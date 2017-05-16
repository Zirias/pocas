#ifndef POCAS_CORE_CMDLINE_H
#define POCAS_CORE_CMDLINE_H

#include <pocas/core/decl.h>

enum PC_CmdlineStyle
{
    PC_CLS_Auto = 0,
    PC_CLS_Unix = 1 << 0,
    PC_CLS_GnuLong = 1 << 1,
    PC_CLS_Windows = 1 << 2,
    PC_CLS_Gnu = PC_CLS_Unix|PC_CLS_GnuLong,
    PC_CLS_Any = PC_CLS_Gnu|PC_CLS_Windows
};
C_ENUM_DECL(PC_CmdlineStyle);

enum PC_CmdlineOptMode
{
    PC_COM_Switch,
    PC_COM_ArgAllowed,
    PC_COM_ArgRequired
};
C_ENUM_DECL(PC_CmdlineOptMode);

C_CLASS_DECL(PC_CmdlineParser);
C_CLASS_DECL(PC_CmdlineItem);
C_CLASS_DECL(PC_Cmdline);

#define OPTID_POSITIONALARG 0

DECLEXPORT PC_Cmdline *PC_Cmdline_create(PC_CmdlineStyle style);
DECLEXPORT int PC_Cmdline_addFromArgv(PC_Cmdline *self, int argc, char **argv);
DECLEXPORT int PC_Cmdline_addFromString(PC_Cmdline *self, const char *args);
DECLEXPORT void PC_Cmdline_rewind(PC_Cmdline *self);
DECLEXPORT void PC_Cmdline_destroy(PC_Cmdline *self);

DECLEXPORT PC_CmdlineParser *PC_CmdlineParser_create(void);
DECLEXPORT int PC_CmdlineParser_register(PC_CmdlineParser *self, int id,
        char shortOpt, const char *longOpt, PC_CmdlineOptMode om);
DECLEXPORT const PC_CmdlineItem *PC_CmdlineParser_next(PC_CmdlineParser *self,
        PC_Cmdline *cmdline);
DECLEXPORT void PC_CmdlineParser_destroy(PC_CmdlineParser *self);

DECLEXPORT int PC_CmdlineItem_id(const PC_CmdlineItem *self);
DECLEXPORT const char *PC_CmdlineItem_arg(const PC_CmdlineItem *self);

#endif
