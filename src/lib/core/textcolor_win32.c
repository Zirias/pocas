#include <stdio.h>
#include <windows.h>

#include "c11threads.h"

#include <pocas/core/textcolor.h>

static const char *ansi[] =
{
    "\x1B[0m",
    "\x1B[0;30m",
    "\x1B[0;34m",
    "\x1B[0;32m",
    "\x1B[0;36m",
    "\x1B[0;31m",
    "\x1B[0;35m",
    "\x1B[0;33m",
    "\x1B[0;37m",
    "\x1B[1;30m",
    "\x1B[1;34m",
    "\x1B[1;32m",
    "\x1B[1;36m",
    "\x1B[1;31m",
    "\x1B[1;35m",
    "\x1B[1;33m",
    "\x1B[1;37m"
};

static once_flag getConsoleAttrFlag = ONCE_FLAG_INIT;
static WORD consoleOutDefaultColor;
static WORD consoleErrDefaultColor;

static void getConsoleAttr(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
    {
        consoleOutDefaultColor = csbi.wAttributes;
    }
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_ERROR_HANDLE), &csbi))
    {
        consoleErrDefaultColor = csbi.wAttributes;
    }
}

SOEXPORT void TextColor_use(TextColor color, ConsoleStream stream)
{
    HANDLE out = GetStdHandle(
                stream == ConsoleStream_ERROR ?
                    STD_ERROR_HANDLE : STD_OUTPUT_HANDLE);
    DWORD outType = GetFileType(out);
    DWORD mode;
    if (outType == FILE_TYPE_CHAR && GetConsoleMode(out, &mode))
    {
        // in native windows console use console API

        call_once(&getConsoleAttrFlag, getConsoleAttr);
        if (color == TextColor_NORMAL) color =
                stream == ConsoleStream_ERROR ?
                    consoleErrDefaultColor : consoleOutDefaultColor;
        SetConsoleTextAttribute(out, color);
    }
    else if (outType == FILE_TYPE_PIPE)
    {
        // through a pipe, hope the receiving end understands ANSI codes.

        fputs(ansi[color+1], stream == ConsoleStream_ERROR ? stderr : stdout);
    }

    // for output to files or other character devices,
    // don't attempt to use colors.
}
