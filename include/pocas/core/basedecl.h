#ifndef POCAS_CORE_BASEDECL_H
#define POCAS_CORE_BASEDECL_H

#undef pocaslibs___cdecl
#undef SOEXPORT
#undef SOLOCAL
#undef SOLOCAL_CDECL
#undef DECLEXPORT

#ifdef __cplusplus
#  define pocaslibs___cdecl extern "C"
#  define DECLDATA
#  define C_CLASS_DECL(t) struct t
#  define C_ENUM_DECL(t) enum t
#else
#  define pocaslibs___cdecl
#  define DECLDATA extern
#  define C_CLASS_DECL(t) typedef struct t t
#  define C_ENUM_DECL(t) typedef enum t t
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
#define SOLOCAL_CDECL pocaslibs___cdecl SOLOCAL

#endif
