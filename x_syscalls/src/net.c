#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include <sys/wait.h>
#include <errno.h>


#define STACKSIZE (1024*1024)
static char child_stack[STACKSIZE];

struct clone_args {
        char **argv;
};

static int child_exec(void *arguments)
{
        struct clone_args *args = (struct clone_args *)arguments;
        if (execvp(args->argv[0], args->argv) != 0) {
                fprintf(stderr, "failed to execute: %s\n", strerror(errno));
                exit(-1);
        }
        exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
        struct clone_args args;
        args.argv = &argv[1];

        int clone_flags = CLONE_NEWNET | SIGCHLD;

        pid_t pid = clone(child_exec, child_stack + STACKSIZE, clone_flags, &args);
        if (pid < 0) {
                fprintf(stderr, "clone failed: %s\n", strerror(errno));
                exit(EXIT_FAILURE);
        }

        if (waitpid(pid, NULL, 0) == -1) {
                fprintf(stderr, "failed to wait pid: %d\n", pid);
                exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
}
