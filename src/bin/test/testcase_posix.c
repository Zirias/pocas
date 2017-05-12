#define _POSIX_C_SOURCE 1
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <pocas/core/list.h>
#include <pocas/core/string.h>

#include "runner_internal.h"
#include "testclass_internal.h"
#include "testcase_internal.h"

static void stripNewline(char *s)
{
    while (*s)
    {
        if (*s == '\n')
        {
            *s = '\0';
            return;
        }
        ++s;
    }
}

void TestCase_run(TestCase *self, const char *gdbPath)
{
    int pipefd[2];
    pipe(pipefd);

    pid_t pid = fork();
    if (pid < 0)
    {
        fprintf(stderr, "Error creating pipe: %s\n", strerror(errno));
        return;
    }

    if (pid)
    {
	close(pipefd[1]);
	FILE *runnerPipe = fdopen(pipefd[0], "r");
	List *output = List_createStr(0);
	char buf[1024];
	char *line;
	while ((line = fgets(buf, 1024, runnerPipe)))
	{
	    stripNewline(line);
	    List_append(output, String_copy(line));
	}

	int status;
	waitpid(pid, &status, 0);
	fclose(runnerPipe);

	int rc = -1;
	if (WIFEXITED(status))
	{
	    rc = WEXITSTATUS(status);
	}
	else if (WIFSIGNALED(status))
	{
	    rc = WTERMSIG(status) << 8;
	}

	TestCase_evaluate(self, rc, output);
    }
    else
    {
	close(pipefd[0]);
	FILE *testPipe = fdopen(pipefd[1], "a");
	setvbuf(testPipe, 0, _IONBF, 0);
	Runner_runTest(testPipe, TestClass_plugin(TestCase_class(self)),
		TestCase_method(self));
    }
}
