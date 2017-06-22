#include <pocas/core/basedecl.h>

#ifdef _WIN32
#  undef DECLEXPORT
#  ifdef BUILDING_POCAS_TEST
#    define DECLEXPORT pocaslibs___cdecl __declspec(dllexport)
#  else
#    define DECLEXPORT pocaslibs___cdecl __declspec(dllimport)
#  endif
#endif

#ifndef TEST_SOEXPORT
#  ifdef _WIN32
#    define TEST_SOEXPORT pocaslibs___cdecl __declspec(dllexport)
#  else
#    define TEST_SOEXPORT
#  endif
#endif

