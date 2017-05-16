#include <pocas/core/basedecl.h>

#ifdef _WIN32
#  undef DECLEXPORT
#  ifdef BUILDING_POCAS_TEST
#    define DECLEXPORT __declspec(dllexport)
#  else
#    define DECLEXPORT pocaslibs___cdecl __declspec(dllimport)
#  endif
#endif
