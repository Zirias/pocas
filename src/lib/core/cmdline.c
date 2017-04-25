#include <stdlib.h>

#include <pocas/core/hashtable.h>
#include <pocas/core/list.h>
#include <pocas/core/string.h>
#include <pocas/core/stringbuilder.h>

#include <pocas/core/cmdline.h>

#ifdef _WIN32
#define CLS_Platform CLS_Windows
#else
#define CLS_Platform CLS_Gnu
#endif

struct Cmdline
{
    CmdlineStyle style;
    int noMoreOpts;
    List *rawItems;
    ListIterator *rawItemsIter;
    int endFound;
};

struct CmdlineItem
{
    int id;
    const char *arg;
};

typedef struct CmdlineOptDescriptor
{
    int id;
    CmdlineOptMode optMode;
} CmdlineOptDescriptor;

struct CmdlineParser
{
    HashTable *shortOpts;
    HashTable *longOpts;
    const char *parsep;
    CmdlineItem item;
};

SOEXPORT Cmdline *Cmdline_create(CmdlineStyle style)
{
    Cmdline *self = malloc(sizeof(Cmdline));
    self->style = style ? style : CLS_Platform;
    self->noMoreOpts = 0;
    self->rawItems = List_createStr(0);
    self->rawItemsIter = 0;
    self->endFound = 0;
    return self;
}

SOEXPORT int Cmdline_addFromArgv(Cmdline *self, int argc, const char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        List_append(self->rawItems, String_copy(argv[i]));
    }
    return 1;
}

enum clreadmode
{
    NORMAL,
    TOKEN,
    ESCAPE,
    QUOTE,
    COMMENT
};

int Cmdline_addFromString(Cmdline *self, const char *args)
{
    enum clreadmode readmode = NORMAL;
    enum clreadmode escmode = NORMAL;
    char quot = 0;
    StringBuilder *itemBuilder = StringBuilder_create(0);
    const char *readp = args;

    while (*readp)
    {
        switch (readmode)
        {
            case NORMAL:
                if (*readp != ' ' && *readp != '\t'
                        && *readp != '\r' && *readp != '\n')
                {
                    readmode = TOKEN;
                }
                else
                {
                    ++readp;
                }
                break;

            case TOKEN:
                if (*readp == '\\')
                {
                    readmode = ESCAPE;
                    escmode = TOKEN;
                    ++readp;
                    break;
                }
                else if (*readp == '"' || *readp == '\'')
                {
                    readmode = QUOTE;
                    quot = *readp;
                    ++readp;
                    break;
                }
                else if (*readp == '#' || *readp == ';')
                {
                    readmode = COMMENT;
                    goto endToken;
                }
                else if (*readp == ' ' || *readp == '\t'
                         || *readp == '\r' || *readp == '\n')
                {
                    readmode = NORMAL;
                    goto endToken;
                }
                StringBuilder_appendChar(itemBuilder, *readp);
                ++readp;
                break;

            endToken:
                List_append(self->rawItems,
                            StringBuilder_toString(itemBuilder));
                StringBuilder_clear(itemBuilder);
                ++readp;
                break;

            case ESCAPE:
                if (*readp != '\r' || readp[1] != '\n')
                {
                    // allow to escape windows line-ending as a whole
                    readmode = escmode;
                }
                StringBuilder_appendChar(itemBuilder, *readp);
                ++readp;
                break;

            case QUOTE:
                if (*readp == quot)
                {
                    readmode = TOKEN;
                }
                else if (*readp == '\\')
                {
                    readmode = ESCAPE;
                    escmode = QUOTE;
                }
                else
                {
                    StringBuilder_appendChar(itemBuilder, *readp);
                }
                ++readp;
                break;

            case COMMENT:
                if (*readp == '\r' || *readp == '\n')
                {
                    readmode = NORMAL;
                }
                ++readp;
                break;
        }
    }

    if (StringBuilder_length(itemBuilder))
    {
        List_append(self->rawItems, StringBuilder_toString(itemBuilder));
    }
    StringBuilder_destroy(itemBuilder);
    return 1;
}

SOEXPORT void Cmdline_rewind(Cmdline *self)
{
    ListIterator_destroy(self->rawItemsIter);
    self->rawItemsIter = List_iterator(self->rawItems);
    self->noMoreOpts = 0;
    self->endFound = 0;
}

SOEXPORT void Cmdline_destroy(Cmdline *self)
{
    if (!self) return;
    ListIterator_destroy(self->rawItemsIter);
    List_destroy(self->rawItems);
    free(self);
}

static void charKeyProvider(HashKey *key, const void *keyObject)
{
    HashKey_set(key, 1, keyObject);
}

SOEXPORT CmdlineParser *CmdlineParser_create(void)
{
    CmdlineParser *self = malloc(sizeof(CmdlineParser));
    self->shortOpts = HashTable_create(
                HashTableSize_32, charKeyProvider, 0, free);
    self->longOpts = HashTable_createStrKey(
                HashTableSize_32, 0, free);
    self->parsep = 0;
    return self;
}

SOEXPORT int CmdlineParser_register(CmdlineParser *self, int id,
        char shortOpt, const char *longOpt, CmdlineOptMode om)
{
    if (!shortOpt && !longOpt) return 0;
    if (shortOpt && HashTable_get(self->shortOpts, &shortOpt)) return 0;
    if (longOpt && HashTable_get(self->longOpts, longOpt)) return 0;
    if (shortOpt)
    {
        CmdlineOptDescriptor *d = malloc(sizeof(CmdlineOptDescriptor));
        d->id = id;
        d->optMode = om;
        HashTable_set(self->shortOpts, &shortOpt, d);
    }
    if (longOpt)
    {
        CmdlineOptDescriptor *d = malloc(sizeof(CmdlineOptDescriptor));
        d->id = id;
        d->optMode = om;
        HashTable_set(self->longOpts, longOpt, d);
    }
    return 1;
}

static const CmdlineItem *parseOpt(CmdlineParser *self, const CmdlineOptDescriptor *d)
{
    self->item.arg = 0;
    if (self->parsep && !*self->parsep) self->parsep = 0;

    if (d)
    {
        self->item.id = d->id;
        if (d->optMode == COM_Switch)
        {
            return &self->item;
        }
        else if (d->optMode == COM_ArgRequired)
        {
            if (!self->parsep) return 0;
            self->item.arg = self->parsep;
            self->parsep = 0;
            return &self->item;
        }
        else
        {
            if (self->parsep)
            {
                self->item.arg = self->parsep;
                self->parsep = 0;
            }
            return &self->item;
        }
    }
    else
    {
        if (!self->parsep) return 0;
        self->item.id = OPTID_POSITIONALARG;
        self->item.arg = self->parsep;
        self->parsep = 0;
        return &self->item;
    }
}

static const char *nextRaw(Cmdline *cmdline)
{
    if (cmdline->endFound) return 0;

    if (!cmdline->rawItemsIter)
    {
        cmdline->rawItemsIter = List_iterator(cmdline->rawItems);
    }

    if (!ListIterator_moveNext(cmdline->rawItemsIter))
    {
        cmdline->endFound = 1;
        return 0;
    }

    return ListIterator_current(cmdline->rawItemsIter);
}

const CmdlineItem *CmdlineParser_next(CmdlineParser *self, Cmdline *cmdline)
{
    const CmdlineOptDescriptor *d = 0;

    if (self->parsep)
    {
        d = HashTable_get(self->shortOpts, self->parsep);
        return parseOpt(self, d);
    }
    else
    {
        self->parsep = nextRaw(cmdline);
        if (!self->parsep) return 0;
    }
    if (*self->parsep == '/' && cmdline->style & CLS_Windows)
    {
        ++self->parsep;
        if (!*self->parsep)
        {
            self->parsep = 0;
            return 0;
        }
        StringBuilder *optBuilder = StringBuilder_create(0);
        while (*self->parsep && *self->parsep != ':')
        {
            StringBuilder_appendChar(optBuilder, *self->parsep);
            ++self->parsep;
        }
        char *opt = StringBuilder_toString(optBuilder);
        if (StringBuilder_length(optBuilder) == 1)
        {
            d = HashTable_get(self->shortOpts, &opt[0]);
        }
        else
        {
            d = HashTable_get(self->longOpts, opt);
        }
        free(opt);
        StringBuilder_destroy(optBuilder);
        if (!*self->parsep) self->parsep = nextRaw(cmdline);
        else ++self->parsep;
        return parseOpt(self, d);
    }
    else if (*self->parsep == '-' && cmdline->style & CLS_Gnu)
    {
        if (!cmdline->noMoreOpts &&
                self->parsep[1] == '-' && cmdline->style & CLS_GnuLong)
        {
            self->parsep += 2;
            if (!*self->parsep)
            {
                cmdline->noMoreOpts = 1;
                self->parsep = 0;
                return 0;
            }
            StringBuilder *optBuilder = StringBuilder_create(0);
            while (*self->parsep && *self->parsep != '=')
            {
                StringBuilder_appendChar(optBuilder, *self->parsep);
                ++self->parsep;
            }
            char *opt = StringBuilder_toString(optBuilder);
            d = HashTable_get(self->longOpts, opt);
            free(opt);
            StringBuilder_destroy(optBuilder);
            if (!*self->parsep) self->parsep = nextRaw(cmdline);
            else ++self->parsep;
            return parseOpt(self, d);
        }
        else if (cmdline->style & CLS_Unix &&
                 (!(cmdline->style & CLS_GnuLong) || !cmdline->noMoreOpts))
        {
            ++self->parsep;
            if (!*self->parsep)
            {
                self->parsep = 0;
                return 0;
            }
            d = HashTable_get(self->shortOpts, self->parsep);
            ++self->parsep;
            if (!*self->parsep) self->parsep = nextRaw(cmdline);
            return parseOpt(self, d);
        }
    }
    return parseOpt(self, 0);
}

SOEXPORT void CmdlineParser_destroy(CmdlineParser *self)
{
    if (!self) return;
    HashTable_destroy(self->longOpts);
    HashTable_destroy(self->shortOpts);
    free(self);
}

SOEXPORT int CmdlineItem_id(const CmdlineItem *self)
{
    return self->id;
}

SOEXPORT const char *CmdlineItem_arg(const CmdlineItem *self)
{
    return self->arg;
}
