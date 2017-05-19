#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

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

static const char *ansiTerms[] =
{
    "xterm",
    "rxvt",
    "vt100",
    "linux",
    "screen",
    0
};

static int isAnsiTerm(void)
{
    char *term = getenv("TERM");
    for (const char **ansiTerm = &ansiTerms[0]; *ansiTerm; ++ansiTerm)
    {
	int match = 1;
	const char *t = term;
	const char *a = *ansiTerm;
	while (*a && *t)
	{
	    if (*a++ != *t++)
	    {
		match = 0;
		break;
	    }
	}
	if (match) return 1;
    }
    return 0;
}

SOEXPORT void PC_TextColor_use(PC_TextColor color, FILE *outStream)
{
    int doColor = 0;
    int outFd = fileno(outStream);
    struct stat st;

    if (isatty(outFd) && isAnsiTerm())
    {
        doColor = 1;
    }
    else if (fstat(outFd, &st) >= 0 && S_ISFIFO(st.st_mode))
    {
        doColor = 1;
    }

    if (doColor)
    {
        fputs(ansi[color+1], outStream);
    }
}
