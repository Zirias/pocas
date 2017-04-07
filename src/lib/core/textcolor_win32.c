#include <stdio.h>
#include <windows.h>

#include <pocas/core/textcolor.h>

static const char *ansi[] =
{
    "\x1B[0m",
    "\x1B[30m",
    "\x1B[34m",
    "\x1B[32m",
    "\x1B[36m",
    "\x1B[31m",
    "\x1B[35m",
    "\x1B[33m",
    "\x1B[37m",
    "\x1B[90m",
    "\x1B[94m",
    "\x1B[92m",
    "\x1B[96m",
    "\x1B[91m",
    "\x1B[95m",
    "\x1B[93m",
    "\x1B[97m"
};

SOEXPORT void TextColor_use(TextColor color, ConsoleStream stream)
{
    char buffer[64];
    if (GetEnvironmentVariable("TERM", buffer, 64))
    {
        if (!strncmp("xterm", buffer, 5) ||
            !strncmp("vt100", buffer, 4) ||
            !strncmp("rxvt", buffer, 4))
        {
            fputs(ansi[color+1], stream == ConsoleStream_ERROR ? stderr : stdout);
        }
    }
    else
    {
        HANDLE out = GetStdHandle(
                    stream == ConsoleStream_ERROR ? STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
        if (color == TextColor_NORMAL) color = TextColor_LIGHTGRAY;
        SetConsoleTextAttribute(out, color);
    }
}
