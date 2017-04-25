#ifndef POCAS_CORE_CMDLINE_H
#define POCAS_CORE_CMDLINE_H

#include <pocas/core/decl.h>

typedef enum CmdlineStyle
{
    CLS_Auto = 0,
    CLS_Unix = 1 << 0,
    CLS_GnuLong = 1 << 1,
    CLS_Windows = 1 << 2,
    CLS_Gnu = CLS_Unix|CLS_GnuLong,
    CLS_Any = CLS_Gnu|CLS_Windows
} CmdlineStyle;

typedef enum CmdlineOptMode
{
    COM_Switch,
    COM_ArgAllowed,
    COM_ArgRequired
} CmdlineOptMode;

typedef struct CmdlineParser CmdlineParser;

typedef struct CmdlineItem CmdlineItem;

typedef struct Cmdline Cmdline;

#define OPTID_POSITIONALARG 0

DECLEXPORT Cmdline *Cmdline_create(CmdlineStyle style);
DECLEXPORT int Cmdline_addFromArgv(Cmdline *self, int argc, const char **argv);
DECLEXPORT int Cmdline_addFromString(Cmdline *self, const char *args);
DECLEXPORT void Cmdline_rewind(Cmdline *self);
DECLEXPORT void Cmdline_destroy(Cmdline *self);

DECLEXPORT CmdlineParser *CmdlineParser_create(void);
DECLEXPORT int CmdlineParser_register(CmdlineParser *self, int id,
        char shortOpt, const char *longOpt, CmdlineOptMode om);
DECLEXPORT const CmdlineItem *CmdlineParser_next(CmdlineParser *self,
        Cmdline *cmdline);
DECLEXPORT void CmdlineParser_destroy(CmdlineParser *self);

DECLEXPORT int CmdlineItem_id(const CmdlineItem *self);
DECLEXPORT const char *CmdlineItem_arg(const CmdlineItem *self);

#endif
