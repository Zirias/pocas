#ifndef BQT_BACKEND_H
#define BQT_BACKEND_H

#include <pocas/gui/private/backend.h>

#include "decl.h"

SOLOCAL_CDECL int Bqt_Window_create(PG_Window *w);

extern PG_Backend *PG_qtBackend;

#endif
