#include <stdlib.h>

#include <pocas/core/hashtable.h>
#include <pocas/core/list.h>
#include <pocas/core/string.h>
#include <pocas/core/stringbuilder.h>

#include <pocas/core/cmdline.h>

#ifdef _WIN32
#define PC_CLS_Platform PC_CLS_Windows
#else
#define PC_CLS_Platform PC_CLS_Gnu
#endif

struct PC_Cmdline
{
    PC_CmdlineStyle style;
    int noMoreOpts;
    PC_List *rawItems;
    PC_ListIterator *rawItemsIter;
    int endFound;
};

struct PC_CmdlineItem
{
    int id;
    const char *arg;
};

typedef struct PC_CmdlineOptDescriptor
{
    int id;
    PC_CmdlineOptMode optMode;
} PC_CmdlineOptDescriptor;

struct PC_CmdlineParser
{
    PC_HashTable *shortOpts;
    PC_HashTable *longOpts;
    const char *parsep;
    PC_CmdlineItem item;
};

SOEXPORT PC_Cmdline *PC_Cmdline_create(PC_CmdlineStyle style)
{
    PC_Cmdline *self = malloc(sizeof(PC_Cmdline));
    self->style = style ? style : PC_CLS_Platform;
    self->noMoreOpts = 0;
    self->rawItems = PC_List_createStr(0);
    self->rawItemsIter = 0;
    self->endFound = 0;
    return self;
}

SOEXPORT int PC_Cmdline_addFromArgv(PC_Cmdline *self, int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        PC_List_append(self->rawItems, PC_String_copy(argv[i]));
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

int PC_Cmdline_addFromString(PC_Cmdline *self, const char *args)
{
    enum clreadmode readmode = NORMAL;
    enum clreadmode escmode = NORMAL;
    char quot = 0;
    PC_StringBuilder *itemBuilder = PC_StringBuilder_create(0);
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
                PC_StringBuilder_appendChar(itemBuilder, *readp);
                ++readp;
                break;

            endToken:
                PC_List_append(self->rawItems,
                            PC_StringBuilder_toString(itemBuilder));
                PC_StringBuilder_clear(itemBuilder);
                ++readp;
                break;

            case ESCAPE:
                if (*readp != '\r' || readp[1] != '\n')
                {
                    // allow to escape windows line-ending as a whole
                    readmode = escmode;
                }
                PC_StringBuilder_appendChar(itemBuilder, *readp);
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
                    PC_StringBuilder_appendChar(itemBuilder, *readp);
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

    if (PC_StringBuilder_length(itemBuilder))
    {
        PC_List_append(self->rawItems, PC_StringBuilder_toString(itemBuilder));
    }
    PC_StringBuilder_destroy(itemBuilder);
    return 1;
}

SOEXPORT void PC_Cmdline_rewind(PC_Cmdline *self)
{
    PC_ListIterator_destroy(self->rawItemsIter);
    self->rawItemsIter = PC_List_iterator(self->rawItems);
    self->noMoreOpts = 0;
    self->endFound = 0;
}

SOEXPORT void PC_Cmdline_destroy(PC_Cmdline *self)
{
    if (!self) return;
    PC_ListIterator_destroy(self->rawItemsIter);
    PC_List_destroy(self->rawItems);
    free(self);
}

static void charKeyProvider(PC_HashKey *key, const void *keyObject)
{
    PC_HashKey_set(key, 1, keyObject);
}

SOEXPORT PC_CmdlineParser *PC_CmdlineParser_create(void)
{
    PC_CmdlineParser *self = malloc(sizeof(PC_CmdlineParser));
    self->shortOpts = PC_HashTable_create(
                PC_HTS_32, charKeyProvider, 0, free);
    self->longOpts = PC_HashTable_createStrKey(
                PC_HTS_32, 0, free);
    self->parsep = 0;
    return self;
}

SOEXPORT int PC_CmdlineParser_register(PC_CmdlineParser *self, int id,
        char shortOpt, const char *longOpt, PC_CmdlineOptMode om)
{
    if (!shortOpt && !longOpt) return 0;
    if (shortOpt && PC_HashTable_get(self->shortOpts, &shortOpt)) return 0;
    if (longOpt && PC_HashTable_get(self->longOpts, longOpt)) return 0;
    if (shortOpt)
    {
        PC_CmdlineOptDescriptor *d = malloc(sizeof(PC_CmdlineOptDescriptor));
        d->id = id;
        d->optMode = om;
        PC_HashTable_set(self->shortOpts, &shortOpt, d);
    }
    if (longOpt)
    {
        PC_CmdlineOptDescriptor *d = malloc(sizeof(PC_CmdlineOptDescriptor));
        d->id = id;
        d->optMode = om;
        PC_HashTable_set(self->longOpts, longOpt, d);
    }
    return 1;
}

static const PC_CmdlineItem *parseOpt(PC_CmdlineParser *self, const PC_CmdlineOptDescriptor *d)
{
    self->item.arg = 0;
    if (self->parsep && !*self->parsep) self->parsep = 0;

    if (d)
    {
        self->item.id = d->id;
        if (d->optMode == PC_COM_Switch)
        {
            return &self->item;
        }
        else if (d->optMode == PC_COM_ArgRequired)
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

static const char *nextRaw(PC_Cmdline *cmdline)
{
    if (cmdline->endFound) return 0;

    if (!cmdline->rawItemsIter)
    {
        cmdline->rawItemsIter = PC_List_iterator(cmdline->rawItems);
    }

    if (!PC_ListIterator_moveNext(cmdline->rawItemsIter))
    {
        cmdline->endFound = 1;
        return 0;
    }

    return PC_ListIterator_current(cmdline->rawItemsIter);
}

const PC_CmdlineItem *PC_CmdlineParser_next(PC_CmdlineParser *self, PC_Cmdline *cmdline)
{
    const PC_CmdlineOptDescriptor *d = 0;

    if (self->parsep)
    {
        d = PC_HashTable_get(self->shortOpts, self->parsep);
        return parseOpt(self, d);
    }
    else
    {
        self->parsep = nextRaw(cmdline);
        if (!self->parsep) return 0;
    }
    if (*self->parsep == '/' && cmdline->style & PC_CLS_Windows)
    {
        ++self->parsep;
        if (!*self->parsep)
        {
            self->parsep = 0;
            return 0;
        }
        PC_StringBuilder *optBuilder = PC_StringBuilder_create(0);
        while (*self->parsep && *self->parsep != ':')
        {
            PC_StringBuilder_appendChar(optBuilder, *self->parsep);
            ++self->parsep;
        }
        char *opt = PC_StringBuilder_toString(optBuilder);
        if (PC_StringBuilder_length(optBuilder) == 1)
        {
            d = PC_HashTable_get(self->shortOpts, &opt[0]);
        }
        else
        {
            d = PC_HashTable_get(self->longOpts, opt);
        }
        free(opt);
        PC_StringBuilder_destroy(optBuilder);
        if (!*self->parsep) self->parsep = nextRaw(cmdline);
        else ++self->parsep;
        return parseOpt(self, d);
    }
    else if (*self->parsep == '-' && cmdline->style & PC_CLS_Gnu)
    {
        if (!cmdline->noMoreOpts &&
                self->parsep[1] == '-' && cmdline->style & PC_CLS_GnuLong)
        {
            self->parsep += 2;
            if (!*self->parsep)
            {
                cmdline->noMoreOpts = 1;
                self->parsep = 0;
                return 0;
            }
            PC_StringBuilder *optBuilder = PC_StringBuilder_create(0);
            while (*self->parsep && *self->parsep != '=')
            {
                PC_StringBuilder_appendChar(optBuilder, *self->parsep);
                ++self->parsep;
            }
            char *opt = PC_StringBuilder_toString(optBuilder);
            d = PC_HashTable_get(self->longOpts, opt);
            free(opt);
            PC_StringBuilder_destroy(optBuilder);
            if (!*self->parsep) self->parsep = nextRaw(cmdline);
            else ++self->parsep;
            return parseOpt(self, d);
        }
        else if (cmdline->style & PC_CLS_Unix &&
                 (!(cmdline->style & PC_CLS_GnuLong) || !cmdline->noMoreOpts))
        {
            ++self->parsep;
            if (!*self->parsep)
            {
                self->parsep = 0;
                return 0;
            }
            d = PC_HashTable_get(self->shortOpts, self->parsep);
            ++self->parsep;
            if (!*self->parsep) self->parsep = nextRaw(cmdline);
            return parseOpt(self, d);
        }
    }
    return parseOpt(self, 0);
}

SOEXPORT void PC_CmdlineParser_destroy(PC_CmdlineParser *self)
{
    if (!self) return;
    PC_HashTable_destroy(self->longOpts);
    PC_HashTable_destroy(self->shortOpts);
    free(self);
}

SOEXPORT int PC_CmdlineItem_id(const PC_CmdlineItem *self)
{
    return self->id;
}

SOEXPORT const char *PC_CmdlineItem_arg(const PC_CmdlineItem *self)
{
    return self->arg;
}
