#include "c11threads.h"

#include <stdio.h>
#include <windows.h>
#include <io.h>

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

SOEXPORT void PC_TextColor_use(PC_TextColor color, FILE *outStream)
{
    int outFd = fileno(outStream);
    HANDLE out = (HANDLE)_get_osfhandle(outFd);
    DWORD outType = GetFileType(out);
    DWORD mode;
    if (outType == FILE_TYPE_CHAR && GetConsoleMode(out, &mode))
    {
        // in native windows console use console API

        call_once(&getConsoleAttrFlag, getConsoleAttr);
        if (color == PC_TC_NORMAL)
        {
            HANDLE err = GetStdHandle(STD_ERROR_HANDLE);
            if (out == err)
            {
                color = consoleErrDefaultColor;
            }
            else
            {
                color = consoleOutDefaultColor;
            }
        }
        SetConsoleTextAttribute(out, color);
    }
    else if (outType == FILE_TYPE_PIPE)
    {
        // through a pipe, hope the receiving end understands ANSI codes.

        fputs(ansi[color+1], outStream);
    }

    // for output to files or other character devices,
    // don't attempt to use colors.
}
