#include <stdlib.h>

#include <pocas/core/textcolor.h>

SOEXPORT int PC_TextColor_fputs(const char *s, FILE *outStream)
{
    const char *c = s;
    while (*c)
    {
        if (*c == '~' && *(c+1))
        {
            ++c;
            if (*c == '~')
            {
                fputc('~', outStream);
            }
            else
            {
                if ((unsigned char)*c > 0xf)
                {
                    PC_TextColor_use(PC_TC_NORMAL, outStream);
                }
                else
                {
                    PC_TextColor_use((PC_TextColor)*c, outStream);
                }
            }
        }
        else
        {
            fputc(*c, outStream);
        }
        ++c;
    }
    return 1;
}

SOEXPORT int PC_TextColor_vfprintf(FILE *outStream, const char *format, va_list ap)
{
    va_list app;
    va_copy(app, ap);
    size_t bufsize = vsnprintf(0, 0, format, app) + 1;
    char *buf = malloc(bufsize);
    if (!buf) return -1;
    int result = vsnprintf(buf, bufsize, format, ap);
    if (result < 0) goto done;
    PC_TextColor_fputs(buf, outStream);
done:
    free(buf);
    return result;
}

SOEXPORT int PC_TextColor_fprintf(FILE *outStream, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = PC_TextColor_vfprintf(outStream, format, ap);
    va_end(ap);
    return result;
}

SOEXPORT int PC_TextColor_vprintf(const char *format, va_list ap)
{
    return PC_TextColor_vfprintf(stdout, format, ap);
}

SOEXPORT int PC_TextColor_printf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = PC_TextColor_vprintf(format, ap);
    va_end(ap);
    return result;
}
