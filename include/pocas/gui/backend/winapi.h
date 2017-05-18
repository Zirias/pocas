#ifndef POCAS_GUI_BACKEND_WINAPI_H
#define POCAS_GUI_BACKEND_WINAPI_H

#include <pocas/gui/backend.h>

#ifdef _WIN32
#  undef DECLEXPORT
#  ifdef BUILDING_POCAS_GUI_WINAPI
#    define DECLEXPORT __declspec(dllexport)
#  else
#    ifdef STATIC_POCAS_GUI_WINAPI
#      define DECLEXPORT pocaslibs___cdecl
#    else
#      define DECLEXPORT pocaslibs___cdecl __declspec(dllimport)
#    endif
#  endif
#endif

DECLEXPORT PG_Backend *PG_winapiBackend;

#endif
