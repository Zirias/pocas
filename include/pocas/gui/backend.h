#ifndef POCAS_GUI_BACKEND_H
#define POCAS_GUI_BACKEND_H

#include <pocas/gui/decl.h>

typedef struct Backend Backend;

DECLEXPORT const Backend *Backend_current(void);
DECLEXPORT void Backend_setCurrent(const Backend *backend);

#endif
