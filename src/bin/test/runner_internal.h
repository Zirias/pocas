#ifndef RUNNER_INTERNAL_H
#define RUNNER_INTERNAL_H

#include <stdio.h>

#include <pocas/test/decl.h>

C_CLASS_DECL(PC_List);
C_CLASS_DECL(PC_Plugin);

C_CLASS_DECL(Runner);

extern char *exeName;

void Runner_mainHook(PC_List *args, char *gdbPath);
void Runner_runTest(FILE *output, const PC_Plugin *testPlugin,
	const char *methodName);

#endif
