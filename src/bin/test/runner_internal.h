#ifndef RUNNER_INTERNAL_H
#define RUNNER_INTERNAL_H

#include <stdio.h>

typedef struct Plugin Plugin;

extern FILE *testPipe;
extern Plugin *runningTest;

void Runner_runMain(int argc, char **argv);
void Runner_runTest(const char *testMethodName);
void Runner_stopTest();
void Runner_launchTest(const char *runnerExe,
                       Plugin *test, const char *testMethodName);

#endif
