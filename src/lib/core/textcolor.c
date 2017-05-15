#include <stdlib.h>

#include <pocas/core/textcolor.h>

SOEXPORT int TextColor_fputs(const char *s, FILE *outStream)
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
                    TextColor_use(TCI_NORMAL, outStream);
                }
                else
                {
                    TextColor_use((TextColor)*c, outStream);
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

SOEXPORT int TextColor_vfprintf(FILE *outStream, const char *format, va_list ap)
{
    size_t bufsize = vsnprintf(0, 0, format, ap) + 1;
    char *buf = malloc(bufsize);
    if (!buf) return -1;
    int result = vsnprintf(buf, bufsize, format, ap);
    if (result < 0) goto done;
    TextColor_fputs(buf, outStream);
done:
    free(buf);
    return result;
}

SOEXPORT int TextColor_fprintf(FILE *outStream, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = TextColor_vfprintf(outStream, format, ap);
    va_end(ap);
    return result;
}

SOEXPORT int TextColor_vprintf(const char *format, va_list ap)
{
    return TextColor_vfprintf(stdout, format, ap);
}

SOEXPORT int TextColor_printf(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = TextColor_vprintf(format, ap);
    va_end(ap);
    return result;
}
