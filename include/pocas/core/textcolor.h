#ifndef POCAS_CORE_TEXTCOLOR_H
#define POCAS_CORE_TEXTCOLOR_H

#include <stdarg.h>
#include <stdio.h>

#include <pocas/core/decl.h>

typedef enum TextColor
{
    TCI_NORMAL = -1,
    TCI_BLACK = 0,
    TCI_BLUE = 1,
    TCI_GREEN = 2,
    TCI_CYAN = 3,
    TCI_RED = 4,
    TCI_MAGENTA = 5,
    TCI_BROWN = 6,
    TCI_LIGHTGRAY = 7,
    TCI_DARKGRAY = 8,
    TCI_LIGHTBLUE = 9,
    TCI_LIGHTGREEN = 10,
    TCI_LIGHTCYAN = 11,
    TCI_LIGHTRED = 12,
    TCI_LIGHTMAGENTA = 13,
    TCI_YELLOW = 14,
    TCI_WHITE = 15
} TextColor;

#define TCS_NORMAL "~\xff"
#define TCS_BLACK "~\x00"
#define TCS_BLUE "~\x01"
#define TCS_GREEN "~\x02"
#define TCS_CYAN "~\x03"
#define TCS_RED "~\x04"
#define TCS_MAGENTA "~\x05"
#define TCS_BROWN "~\x06"
#define TCS_LIGHTGRAY "~\x07"
#define TCS_DARKGRAY "~\x08"
#define TCS_LIGHTBLUE "~\x09"
#define TCS_LIGHTGREEN "~\x0a"
#define TCS_LIGHTCYAN "~\x0b"
#define TCS_LIGHTRED "~\x0c"
#define TCS_LIGHTMAGENTA "~\x0d"
#define TCS_YELLOW "~\x0e"
#define TCS_WHITE "~\x0f"

DECLEXPORT void TextColor_use(TextColor color, FILE *outStream);
DECLEXPORT int TextColor_fputs(const char *s, FILE *outStream);
DECLEXPORT int TextColor_vfprintf(FILE *outStream, const char *format, va_list ap);
DECLEXPORT int TextColor_fprintf(FILE *outStream, const char *format, ...);
DECLEXPORT int TextColor_vprintf(const char *format, va_list ap);
DECLEXPORT int TextColor_printf(const char *format, ...);

#endif
