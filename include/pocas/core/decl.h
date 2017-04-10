#ifndef POCAS_CORE_DECL_H
#define POCAS_CORE_DECL_H

#ifdef __cplusplus
#  define pocaslibs___cdecl extern "C"
#else
#  define pocaslibs___cdecl
#endif

#ifndef __GNUC__
#  define __attribute__(x)
#endif

#ifdef _WIN32
#  define SOEXPORT pocaslibs___cdecl __declspec(dllexport)
#  define SOLOCAL
#else
#  define DECLEXPORT pocaslibs___cdecl
#  if __GNUC__ >= 4
#    define SOEXPORT pocaslibs___cdecl __attribute__ ((visibility ("default")))
#    define SOLOCAL __attribute__ ((visibility ("hidden")))
#  else
#    define SOEXPORT pocaslibs___cdecl
#    define SOLOCAL
#  endif
#endif

#endif // POCAS_CORE_DECL_H

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

