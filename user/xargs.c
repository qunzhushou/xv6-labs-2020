#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void run(char *cmd, char **argv)
{
    int pid = fork();
    if (pid < 0)
    {
        fprintf(2, "fork failed\n");
        exit(1);
    }
    if (pid == 0)
    {
        // 子进程
        exec(cmd, argv);
        fprintf(2, "exec failed\n"); // exec会替换当前进程，所以如果exec成功，这句话不会被执行
        exit(1);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "Usage: xargs command\n");
        exit(1);
    }

    char buf[1024];         // 存储从标准输入读到的数据
    char *p = buf;          // 遍历buf
    char *last_param = buf; // 记录每个参数的起始位置
    char *argsbuf[128];     // 存储全部参数
    char **args = argsbuf;  // 遍历argsbuf

    char *cmd = argv[1];

    for (int i = 1; i < argc; i++)
    {
        *args++ = argv[i];
    }

    char **args_xargs = args; // 记录从标准输入读到的的参数部分, 和xargs的参数区分开, 因为每一行都要合并

    while (read(0, p, 1) > 0)
    {
        if (*p == ' ' || *p == '\n')
        {
            // 读到空格或者换行符，表示一个参数读取完成
            *p = '\0';                  // 用'\0'替换空格或者换行符
            *args_xargs++ = last_param; // 记录参数的起始位置
            last_param = p + 1;         // 记录下一个参数的起始位置

            if (*p == '\n')
            {
                // 读到换行符，表示一行所有参数读取完成
                *args_xargs = 0; // 参数列表结束
                run(cmd, argsbuf);
                args_xargs = args; // 重置参数列表
            }
        }
        p++;
    }

    if (args_xargs != args)
    {
        // 最后一行没有换行符
        *p = '\0';
        *args_xargs++ = last_param;
        *args_xargs = 0;
        run(cmd, argsbuf);
    }
    while (wait(0) >= 0)
        ;

    exit(0);
}