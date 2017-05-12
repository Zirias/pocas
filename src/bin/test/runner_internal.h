#ifndef RUNNER_INTERNAL_H
#define RUNNER_INTERNAL_H

#include <stdio.h>

typedef struct List List;
typedef struct Plugin Plugin;
typedef struct Runner Runner;

extern char *exeName;

void Runner_mainHook(List *args, char *gdbPath);
void Runner_runTest(FILE *output, const Plugin *testPlugin,
	const char *methodName);

#endif
