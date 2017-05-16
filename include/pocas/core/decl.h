#include <pocas/core/basedecl.h>

#ifdef _WIN32
#  undef DECLEXPORT
#  ifdef BUILDING_POCAS_CORE
#    define DECLEXPORT __declspec(dllexport)
#  else
#    ifdef STATIC_POCAS_CORE
#      define DECLEXPORT pocaslibs___cdecl
#    else
#      define DECLEXPORT pocaslibs___cdecl __declspec(dllimport)
#    endif
#  endif
#endif

