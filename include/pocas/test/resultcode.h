#ifndef POCAS_TEST_RESULTCODE_H
#define POCAS_TEST_RESULTCODE_H

#include <pocas/test/decl.h>

enum PT_TestResultCode
{
    PT_TRC_CRSH = -2,
    PT_TRC_FAIL = -1,
    PT_TRC_UNKN = 0,
    PT_TRC_PASS = 1,
    PT_TRC_NONE = 2
};
C_ENUM_DECL(PT_TestResultCode);

#endif
