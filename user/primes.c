#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void getPrime(int pipeleftfd[2])
{
    close(pipeleftfd[1]); // 子进程不需要这个管道的写端
    int prime;
    read(pipeleftfd[0], &prime, sizeof(prime));
    if (prime == -1)
    {
        exit(0);
    }
    fprintf(1, "prime %d\n", prime);
    int piperightfd[2]; // 用于接收下一个质数的管道
    if (pipe(piperightfd) == -1)
    {
        fprintf(2, "pipe failed\n");
        exit(1);
    }
    if (fork() != 0)
    {
        // 父进程
        close(piperightfd[0]); // 父进程不需要这个管道的读端
        int num;
        while (read(pipeleftfd[0], &num, sizeof(num)) && num != -1)
        {
            if (num % prime != 0)
            {
                write(piperightfd[1], &num, sizeof(num));
            }
        }
        write(piperightfd[1], &num, sizeof(num)); // 传递-1给下一个进程
        wait(0);
        exit(0);
    }
    else
    {
        // 子进程
        close(pipeleftfd[0]); // 子进程不需要这个管道的读端
        getPrime(piperightfd);
    }
}

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        fprintf(2, "Usage: primes\n");
        exit(1);
    }

    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        fprintf(2, "pipe failed\n");
        exit(1);
    }

    if (fork() != 0)
    {
        // 父进程
        close(pipefd[0]);
        for (int i = 2; i <= 35; i++)
        {
            write(pipefd[1], &i, sizeof(i));
        }
        int end = -1;
        write(pipefd[1], &end, sizeof(end));
        close(pipefd[1]);
        wait(0);
    }
    else
    {
        // 子进程
        close(pipefd[1]);
        getPrime(pipefd);
        exit(0);
    }

    wait(0);
    exit(0);
}