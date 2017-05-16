#ifndef POCAS_CORE_TEXTCOLOR_H
#define POCAS_CORE_TEXTCOLOR_H

#include <stdarg.h>
#include <stdio.h>

#include <pocas/core/decl.h>

enum PC_TextColor
{
    PC_TC_NORMAL = -1,
    PC_TC_BLACK = 0,
    PC_TC_BLUE = 1,
    PC_TC_GREEN = 2,
    PC_TC_CYAN = 3,
    PC_TC_RED = 4,
    PC_TC_MAGENTA = 5,
    PC_TC_BROWN = 6,
    PC_TC_LIGHTGRAY = 7,
    PC_TC_DARKGRAY = 8,
    PC_TC_LIGHTBLUE = 9,
    PC_TC_LIGHTGREEN = 10,
    PC_TC_LIGHTCYAN = 11,
    PC_TC_LIGHTRED = 12,
    PC_TC_LIGHTMAGENTA = 13,
    PC_TC_YELLOW = 14,
    PC_TC_WHITE = 15
};
C_ENUM_DECL(PC_TextColor);

#define PC_TCS_NORMAL "~\xff"
#define PC_TCS_BLACK "~\x00"
#define PC_TCS_BLUE "~\x01"
#define PC_TCS_GREEN "~\x02"
#define PC_TCS_CYAN "~\x03"
#define PC_TCS_RED "~\x04"
#define PC_TCS_MAGENTA "~\x05"
#define PC_TCS_BROWN "~\x06"
#define PC_TCS_LIGHTGRAY "~\x07"
#define PC_TCS_DARKGRAY "~\x08"
#define PC_TCS_LIGHTBLUE "~\x09"
#define PC_TCS_LIGHTGREEN "~\x0a"
#define PC_TCS_LIGHTCYAN "~\x0b"
#define PC_TCS_LIGHTRED "~\x0c"
#define PC_TCS_LIGHTMAGENTA "~\x0d"
#define PC_TCS_YELLOW "~\x0e"
#define PC_TCS_WHITE "~\x0f"

DECLEXPORT void PC_TextColor_use(PC_TextColor color, FILE *outStream);
DECLEXPORT int PC_TextColor_fputs(const char *s, FILE *outStream);
DECLEXPORT int PC_TextColor_vfprintf(FILE *outStream, const char *format, va_list ap);
DECLEXPORT int PC_TextColor_fprintf(FILE *outStream, const char *format, ...);
DECLEXPORT int PC_TextColor_vprintf(const char *format, va_list ap);
DECLEXPORT int PC_TextColor_printf(const char *format, ...);

#endif
