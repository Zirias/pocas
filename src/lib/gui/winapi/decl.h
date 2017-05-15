#ifndef DECL_H
#define DECL_H

#undef pocaslibs___cdecl
#undef SOEXPORT
#undef SOLOCAL
#undef DECLEXPORT

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
#    define SOEXPORT pocaslibs___cdecl __attribute__((visibility("default")))
#    define SOLOCAL __attribute__((visibility("hidden")))
#  else
#    define SOEXPORT pocaslibs___cdecl
#    define SOLOCAL
#  endif
#endif

#endif

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
