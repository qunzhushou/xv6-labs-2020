#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        fprintf(2, "Usage: pingpong\n");
        exit(1);
    }

    int pipefdp2c[2]; // parent to child
    int pipefdc2p[2]; // child to parent
    if (pipe(pipefdp2c) == -1)
    {
        fprintf(2, "pipe failed\n");
        exit(1);
    }
    if (pipe(pipefdc2p) == -1)
    {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid == -1)
    {
        fprintf(2, "fork failed\n");
        exit(1);
    }

    if (pid == 0)
    {
        // 子进程
        char buf;
        read(pipefdp2c[0], &buf, 1);
        fprintf(1, "%d: received ping\n", getpid());

        write(pipefdc2p[1], &buf, 1);
        close(pipefdc2p[1]);
        close(pipefdp2c[0]);
    }
    else
    {
        // 父进程
        char buf = 'a';
        write(pipefdp2c[1], &buf, 1);
        close(pipefdp2c[1]);

        read(pipefdc2p[0], &buf, 1);
        fprintf(1, "%d: received pong\n", getpid());
        close(pipefdc2p[0]);

        wait(0);
    }

    exit(0);
}