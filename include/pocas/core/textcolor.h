#ifndef POCAS_CORE_TEXTCOLOR_H
#define POCAS_CORE_TEXTCOLOR_H

#include <pocas/core/decl.h>

typedef enum TextColor
{
    TextColor_NORMAL = -1,
    TextColor_BLACK = 0,
    TextColor_BLUE = 1,
    TextColor_GREEN = 2,
    TextColor_CYAN = 3,
    TextColor_RED = 4,
    TextColor_MAGENTA = 5,
    TextColor_BROWN = 6,
    TextColor_LIGHTGRAY = 7,
    TextColor_DARKGRAY = 8,
    TextColor_LIGHTBLUE = 9,
    TextColor_LIGHTGREEN = 10,
    TextColor_LIGHTCYAN = 11,
    TextColor_LIGHTRED = 12,
    TextColor_LIGHTMAGENTA = 13,
    TextColor_YELLOW = 14,
    TextColor_WHITE = 15
} TextColor;

typedef enum ConsoleStream
{
    ConsoleStream_OUT = 1,
    ConsoleStream_ERROR = 2
} ConsoleStream;

DECLEXPORT void TextColor_use(TextColor color, ConsoleStream stream);

#endif
