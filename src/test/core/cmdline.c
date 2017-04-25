#include <pocas/core/cmdline.h>
#include <pocas/test/test.h>

TESTCLASS(pocascore::Cmdline);

static CmdlineParser *parser;

#define OPTID_VERSION 0x101
#define OPTID_HELP 0x102
#define OPTID_FOO 0x103
#define OPTID_BAR 0x104
#define OPTID_FROB 0x105
#define OPTID_SHORT 0x106
#define OPTID_LONG 0x107

TESTINIT()
{
    parser = CmdlineParser_create();
    CmdlineParser_register(parser, OPTID_VERSION, 'v', "version", COM_Switch);
    CmdlineParser_register(parser, OPTID_HELP, '?', "help", COM_Switch);
    CmdlineParser_register(parser, OPTID_FOO, 'f', "foo", COM_ArgAllowed);
    CmdlineParser_register(parser, OPTID_BAR, 'b', "bar", COM_ArgAllowed);
    CmdlineParser_register(parser, OPTID_FROB, 'r', "frob", COM_ArgRequired);
    CmdlineParser_register(parser, OPTID_SHORT, 's', 0, COM_ArgRequired);
    CmdlineParser_register(parser, OPTID_LONG, 0, "long", COM_Switch);
}

TESTDONE()
{
    CmdlineParser_destroy(parser);
}

TESTMETHOD(finds_single_switch_win)
{
    static const char *goodline1 = "/v";
    static const char *goodline2 = "foo /v bar";
    static const char *goodline3 = "/version";
    static const char *badline1 = "blarg /nil";
    static const char *badline2 = "-v test";

    Cmdline *cl;
    const CmdlineItem *item;
    int found;

    found = 0;
    cl = Cmdline_create(CLS_Windows);
    Cmdline_addFromString(cl, goodline1);
    while ((item = CmdlineParser_next(parser, cl)))
    {
        if (CmdlineItem_id(item) == OPTID_VERSION) ++found;
    }
    Cmdline_destroy(cl);
    Test_assertIntEqual(1, found, "wrong number of version switches found");

    found = 0;
    cl = Cmdline_create(CLS_Windows);
    Cmdline_addFromString(cl, goodline2);
    while ((item = CmdlineParser_next(parser, cl)))
    {
        if (CmdlineItem_id(item) == OPTID_VERSION) ++found;
    }
    Cmdline_destroy(cl);
    Test_assertIntEqual(1, found, "wrong number of version switches found");

    found = 0;
    cl = Cmdline_create(CLS_Windows);
    Cmdline_addFromString(cl, goodline3);
    while ((item = CmdlineParser_next(parser, cl)))
    {
        if (CmdlineItem_id(item) == OPTID_VERSION) ++found;
    }
    Cmdline_destroy(cl);
    Test_assertIntEqual(1, found, "wrong number of version switches found");

    found = 0;
    cl = Cmdline_create(CLS_Windows);
    Cmdline_addFromString(cl, badline1);
    while ((item = CmdlineParser_next(parser, cl)))
    {
        if (CmdlineItem_id(item) == OPTID_VERSION) ++found;
    }
    Cmdline_destroy(cl);
    Test_assertIntEqual(0, found, "wrong number of version switches found");

    found = 0;
    cl = Cmdline_create(CLS_Windows);
    Cmdline_addFromString(cl, badline2);
    while ((item = CmdlineParser_next(parser, cl)))
    {
        if (CmdlineItem_id(item) == OPTID_VERSION) ++found;
    }
    Cmdline_destroy(cl);
    Test_assertIntEqual(0, found, "wrong number of version switches found");

    Test_pass();
}

TESTMETHOD(required_arg_found)
{
    static const char *testargswin = "/frob:test";
    static const char *testargsux = "-rtest";
    static const char *testargsgnu = "--frob test";

    const CmdlineItem *item;

    Cmdline *wincl = Cmdline_create(CLS_Windows);
    Cmdline_addFromString(wincl, testargswin);
    item = CmdlineParser_next(parser, wincl);
    Test_assertNotNull(item, "commandline item not found");
    Test_assertIntEqual(OPTID_FROB, CmdlineItem_id(item), "'frob' option not found");
    Test_assertStrEqual("test", CmdlineItem_arg(item), "unexpected 'frob' argument");
    Cmdline_destroy(wincl);

    Cmdline *uxcl = Cmdline_create(CLS_Unix);
    Cmdline_addFromString(uxcl, testargsux);
    item = CmdlineParser_next(parser, uxcl);
    Test_assertNotNull(item, "commandline item not found");
    Test_assertIntEqual(OPTID_FROB, CmdlineItem_id(item), "'frob' option not found");
    Test_assertStrEqual("test", CmdlineItem_arg(item), "unexpected 'frob' argument");
    Cmdline_destroy(uxcl);

    Cmdline *gnucl = Cmdline_create(CLS_Gnu);
    Cmdline_addFromString(gnucl, testargsgnu);
    item = CmdlineParser_next(parser, gnucl);
    Test_assertNotNull(item, "commandline item not found");
    Test_assertIntEqual(OPTID_FROB, CmdlineItem_id(item), "'frob' option not found");
    Test_assertStrEqual("test", CmdlineItem_arg(item), "unexpected 'frob' argument");
    Cmdline_destroy(gnucl);

    Test_pass();
}
