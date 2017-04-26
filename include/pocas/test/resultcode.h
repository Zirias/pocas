#ifndef POCAS_TEST_RESULTCODE_H
#define POCAS_TEST_RESULTCODE_H

typedef enum TestResultCode
{
    TRC_CRSH = -2,
    TRC_FAIL = -1,
    TRC_UNKN = 0,
    TRC_PASS = 1,
    TRC_NONE = 2
} TestResultCode;

#endif
