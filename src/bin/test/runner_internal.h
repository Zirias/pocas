#ifndef RUNNER_INTERNAL_H
#define RUNNER_INTERNAL_H

#include <stdio.h>

typedef struct List List;
typedef struct Plugin Plugin;
typedef struct Runner Runner;

extern char *exeName;
extern FILE *testPipe;
extern Plugin *runningTest;

void Runner_mainHook(List *args, char *gdbPath);
void Runner_runTest(const char *testMethodName);

#endif
