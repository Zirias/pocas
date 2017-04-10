#ifndef RUNNER_INTERNAL_H
#define RUNNER_INTERNAL_H

#include <stdio.h>

typedef struct List List;
typedef struct Plugin Plugin;

extern FILE *testPipe;
extern Plugin *runningTest;
extern const char *gdb;

void Runner_runMain(int argc, char **argv);
void Runner_runTest(const char *testMethodName);
void Runner_launchTest(const char *runnerExe,
                       Plugin *test, const char *testMethodName);
void Runner_evaluateTest(const char *testMethodName, int exitCode, List *result);
int Runner_evaluateFinal(void);

#endif
