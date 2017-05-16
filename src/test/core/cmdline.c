#include <pocas/core/cmdline.h>
#include <pocas/test/test.h>

PT_TESTCLASS(pocascore::Cmdline);

static PC_CmdlineParser *parser;

#define OPTID_VERSION 0x101
#define OPTID_HELP 0x102
#define OPTID_FOO 0x103
#define OPTID_BAR 0x104
#define OPTID_FROB 0x105
#define OPTID_SHORT 0x106
#define OPTID_LONG 0x107

PT_TESTINIT()
{
    parser = PC_CmdlineParser_create();
    PC_CmdlineParser_register(parser, OPTID_VERSION, 'v', "version", PC_COM_Switch);
    PC_CmdlineParser_register(parser, OPTID_HELP, '?', "help", PC_COM_Switch);
    PC_CmdlineParser_register(parser, OPTID_FOO, 'f', "foo", PC_COM_ArgAllowed);
    PC_CmdlineParser_register(parser, OPTID_BAR, 'b', "bar", PC_COM_ArgAllowed);
    PC_CmdlineParser_register(parser, OPTID_FROB, 'r', "frob", PC_COM_ArgRequired);
    PC_CmdlineParser_register(parser, OPTID_SHORT, 's', 0, PC_COM_ArgRequired);
    PC_CmdlineParser_register(parser, OPTID_LONG, 0, "long", PC_COM_Switch);
}

PT_TESTDONE()
{
    PC_CmdlineParser_destroy(parser);
}

PT_TESTMETHOD(finds_single_switch_win)
{
    static const char *goodline1 = "/v";
    static const char *goodline2 = "foo /v bar";
    static const char *goodline3 = "/version";
    static const char *badline1 = "blarg /nil";
    static const char *badline2 = "-v test";

    PC_Cmdline *cl;
    const PC_CmdlineItem *item;
    int found;

    found = 0;
    cl = PC_Cmdline_create(PC_CLS_Windows);
    PC_Cmdline_addFromString(cl, goodline1);
    while ((item = PC_CmdlineParser_next(parser, cl)))
    {
        if (PC_CmdlineItem_id(item) == OPTID_VERSION) ++found;
    }
    PC_Cmdline_destroy(cl);
    PT_Test_assertIntEqual(1, found, "wrong number of version switches found");

    found = 0;
    cl = PC_Cmdline_create(PC_CLS_Windows);
    PC_Cmdline_addFromString(cl, goodline2);
    while ((item = PC_CmdlineParser_next(parser, cl)))
    {
        if (PC_CmdlineItem_id(item) == OPTID_VERSION) ++found;
    }
    PC_Cmdline_destroy(cl);
    PT_Test_assertIntEqual(1, found, "wrong number of version switches found");

    found = 0;
    cl = PC_Cmdline_create(PC_CLS_Windows);
    PC_Cmdline_addFromString(cl, goodline3);
    while ((item = PC_CmdlineParser_next(parser, cl)))
    {
        if (PC_CmdlineItem_id(item) == OPTID_VERSION) ++found;
    }
    PC_Cmdline_destroy(cl);
    PT_Test_assertIntEqual(1, found, "wrong number of version switches found");

    found = 0;
    cl = PC_Cmdline_create(PC_CLS_Windows);
    PC_Cmdline_addFromString(cl, badline1);
    while ((item = PC_CmdlineParser_next(parser, cl)))
    {
        if (PC_CmdlineItem_id(item) == OPTID_VERSION) ++found;
    }
    PC_Cmdline_destroy(cl);
    PT_Test_assertIntEqual(0, found, "wrong number of version switches found");

    found = 0;
    cl = PC_Cmdline_create(PC_CLS_Windows);
    PC_Cmdline_addFromString(cl, badline2);
    while ((item = PC_CmdlineParser_next(parser, cl)))
    {
        if (PC_CmdlineItem_id(item) == OPTID_VERSION) ++found;
    }
    PC_Cmdline_destroy(cl);
    PT_Test_assertIntEqual(0, found, "wrong number of version switches found");

    PT_Test_pass();
}

PT_TESTMETHOD(required_arg_found)
{
    static const char *testargswin = "/frob:test";
    static const char *testargsux = "-rtest";
    static const char *testargsgnu = "--frob test";

    const PC_CmdlineItem *item;

    PC_Cmdline *wincl = PC_Cmdline_create(PC_CLS_Windows);
    PC_Cmdline_addFromString(wincl, testargswin);
    item = PC_CmdlineParser_next(parser, wincl);
    PT_Test_assertNotNull(item, "commandline item not found");
    PT_Test_assertIntEqual(OPTID_FROB, PC_CmdlineItem_id(item), "'frob' option not found");
    PT_Test_assertStrEqual("test", PC_CmdlineItem_arg(item), "unexpected 'frob' argument");
    PC_Cmdline_destroy(wincl);

    PC_Cmdline *uxcl = PC_Cmdline_create(PC_CLS_Unix);
    PC_Cmdline_addFromString(uxcl, testargsux);
    item = PC_CmdlineParser_next(parser, uxcl);
    PT_Test_assertNotNull(item, "commandline item not found");
    PT_Test_assertIntEqual(OPTID_FROB, PC_CmdlineItem_id(item), "'frob' option not found");
    PT_Test_assertStrEqual("test", PC_CmdlineItem_arg(item), "unexpected 'frob' argument");
    PC_Cmdline_destroy(uxcl);

    PC_Cmdline *gnucl = PC_Cmdline_create(PC_CLS_Gnu);
    PC_Cmdline_addFromString(gnucl, testargsgnu);
    item = PC_CmdlineParser_next(parser, gnucl);
    PT_Test_assertNotNull(item, "commandline item not found");
    PT_Test_assertIntEqual(OPTID_FROB, PC_CmdlineItem_id(item), "'frob' option not found");
    PT_Test_assertStrEqual("test", PC_CmdlineItem_arg(item), "unexpected 'frob' argument");
    PC_Cmdline_destroy(gnucl);

    PT_Test_pass();
}
