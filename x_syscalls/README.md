# Containers do not exist

Containers are actually made of combining various syscalls around namespaces in the Linux Kernel.

## Setup

Run `docker build -t local/docker-syscalls .` inside this directory to build the base image, then `docker run -it --privileged --net host local/docker-syscalls` to enter the container.
The priviledged mode ensures we have priviledges to create new namespaces from within a docker container.
The host network is attached so that we can see the host network stack from inside.

## Base program

We use a little C program that executes any commands you pass to it inside a child process.
It does that using the `clone` syscall.
The program looks like this and is in `base.c`:

```c
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

    int clone_flags = SIGCHLD;

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
```

Compile with `gcc -o base base.c` and then run commands like so: `./base ls -la`.

## Network namespaces

If you run `ip addr` as well as `./base ip addr`, you see no difference in output.
Each time you see the full network stack.

Let's create a new namespace and see what the difference will be.
Change the close_flags to `int clone_flags = CLONE_NEWNET | SIGCHLD;` like it's done in `net.c` and recompile with `gcc -o out net.c`.
Now run `./out ip addr` and you see a reduced network stack.
This is because the flag caused to create a new namespace during clone.

## Mount namespace

Let's do the same for file system mounts.
Compare `mount` and `./base mount` and again no difference.

Now modify the program as follows. Add a new flag like so: `int clone_flags = CLONE_NEWNS | CLONE_NEWNET | SIGCHLD;`.
Also add the following lines in the function `child_exec` before executing the command:
```c
if (mount("none", "/tmp", "tmpfs", 0, "") != 0) {
        fprintf(stderr, "failed to mount tmpfs: %s\n", strerror(errno));
        exit(-1);
}
```

If you now compile (`gcc -o out net_mount.c`) and run `./out mount`, you see an additional mount.
If you run `mount` afterwards, this mount is gone.

Note: the fact that you see all host mounts inside the namespace is a particularity due to the fact that we still see `/proc` from within the namespace, thereby somehow inheriting the mounts. Not sure how to stop this... (maybe google from here: http://crosbymichael.com/creating-containers-part-1.html)

## UTS namespace

Next we isolate information related to system identification.
This includes the hostname, which is again the same regardless of how we call it: `hostname` vs `./base hostname`.

We add another flag `int clone_flags = CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWNET | SIGCHLD;`.
Now we can change the hostname inside our program without affecting the host:
```c
...
#include <unistd.h>
#include <string.h>
...

static int child_exec(void *arguments)
{
        ...
    const char * hn = "custom_hostname";
    if (sethostname(hn, strlen(hn)) != 0) {
        fprintf(stderr, "failed to change hostname: %s\n", strerror(errno));
        exit(-1);
    }
}
...
```

We compile `gcc -o out net_mount_utc.c` and run `./out hostname` to see it is now `custom_hostname`.
We can also make sure with `hostname` that our host still has the old name.

## IPC namespace

In Linux, you can use various mechanisms for inter process communication, e.g., message queues.
List message queues with `ipcs -q`, create them with `ipcmk -Q`, then delete them with `ipcrm -Q <key>`.

Like always, listing queus with `ipcs -q` and `./base ipcs -Q` gives the same results, but we can separate it with namespaces.
Add another flag `int clone_flags = CLONE_NEWIPC | CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWNET | SIGCHLD;` and try it out (`gcc -o out net_mount_utc_ipc.c`):

First do `ipcmk -Q` to create a queue, then compare `ipcs -Q` and `./out ipcs -Q`. You will not see the queue inside the namespace.
Also, creating a queue inside it like so `./out /bin/sh -c 'ipcmk -Q && ipcs -q'` will show a queue not visible from host with `ipcs -q`

# PID namespace

Finally, we will also separate the process namespace.
Right now, there is no difference between `ps auxf` and `./base ps auxf`.

However, we add ` int clone_flags = CLONE_NEWPID | CLONE_NEWIPC | CLONE_NEWUTS | CLONE_NEWNS | CLONE_NEWNET | SIGCHLD;` and also mount a new procfs inside `child_exec`:
```
if (umount("/proc", 0) != 0) {
    fprintf(stderr, "failed unmount /proc %s\n", strerror(errno));
    exit(-1);
}
if (mount("proc", "/proc", "proc", 0, "") != 0) {
    fprintf(stderr, "failed mount /proc %s\n", strerror(errno));
    exit(-1);
}
```
Recompile (`gcc -o out net_mount_utc_ipc_pid.c`) and watch how `./out ps auxf` shows only a single process with PID 1.

