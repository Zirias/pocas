#ifndef POCAS_GUI_BACKEND_H
#define POCAS_GUI_BACKEND_H

#include <pocas/gui/decl.h>

C_CLASS_DECL(PG_Backend);

#ifndef POCAS_GUI_PRIVATE

DECLEXPORT const PG_Backend *PG_Backend_current(void);
DECLEXPORT void PG_Backend_setCurrent(PG_Backend *backend);

#endif

#endif
