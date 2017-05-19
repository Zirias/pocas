#ifndef POCAS_GUI_BACKEND_QT_H
#define POCAS_GUI_BACKEND_QT_H

#include <pocas/gui/backend.h>

#ifdef _WIN32
#  undef DECLEXPORT
#  ifdef BUILDING_POCAS_GUI_QT
#    define DECLEXPORT pocaslibs___cdecl __declspec(dllexport)
#  else
#    ifdef STATIC_POCAS_GUI_QT
#      define DECLEXPORT pocaslibs___cdecl
#    else
#      define DECLEXPORT pocaslibs___cdecl __declspec(dllimport)
#    endif
#  endif
#endif

DECLDATA DECLEXPORT PG_Backend *PG_qtBackend;

#endif
