// MyShell.
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#define MAXARGS 10
#define MAXLEN 20
#define MAXIN 100

// 打印提示符‘@’，读入cmd（参考sh.c中的读入方法）
int n_getcmd(char *buf, int nbuf)
{
    fprintf(2, "@ ");
    memset(buf, 0, nbuf);
    gets(buf, nbuf);
    if (buf[0] == 0)
        return -1;
    return 0;
}

// 处理cmd到argv数组中
void n_set(char *cmd, char* argv[], int* argc)
{
    // 定义要跳过的空格（仿照sh.h）
    char needJump[] = " \t\r\n\v";  
    char args[MAXARGS][MAXLEN];
    // 让argv的每一个元素都指向args的每一行
    for(int m=0; m < MAXARGS; m++)
        argv[m] = &args[m][0];

    int i = 0;
    int j = 0;
    for (; cmd[j] != '\n' && cmd[j] != '\0'; j++)
    {
        // 跳过空格
        while (strchr(needJump, cmd[j]))
            j++;
        argv[i++] = cmd + j;
        // 只要不是空格，就j++,找到下一个空格
        while (strchr(needJump, cmd[j]) == 0)
            j++;

        cmd[j] = '\0';
    }
    argv[i] = 0;
    *argc = i;
}

void n_pipe(char*argv[], int argc);

void n_runcmd(char*argv[], int argc)
{
    for(int i = 1; i < argc; i++){
        if(!strcmp(argv[i], "|"))
            n_pipe(argv, argc);
    }
    for(int i = 1; i < argc; i++){
        // > 输出重定向，关闭标准输出，输出重定向到对应的文件里
        if(!strcmp(argv[i], ">")){
            close(1);
            open(argv[i+1], O_CREATE|O_WRONLY);
            argv[i] = 0;
        }
        // < 输入重定向，关闭标准输入
        if(!strcmp(argv[i], "<")){
            close(0);
            open(argv[i+1], O_RDONLY);
            argv[i] = 0;
        }
    }
    exec(argv[0], argv);
}

// 执行管道
void n_pipe(char*argv[], int argc){
    int i = 0;
    int fd[2];
    pipe(fd);
    // 把cmd中的 | 换成空字符
    for(; i < argc; i++){
        if(!strcmp(argv[i], "|")){
            argv[i] = 0;
            break;
        }
    }
    if(fork()==0){
        // 子进程：执行左边的命令，关闭自身标准输出
        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        n_runcmd(argv,i);
    }
    else{
        // 父进程：执行右边的命令，关闭自身标准输入
        close(0);
        dup(fd[0]);
        close(fd[0]);
        close(fd[1]);
        n_runcmd(argv+i+1, argc-i-1);
    }
}

// 模仿sh.c部分写法
int main()
{
    char buf[MAXIN];
    while (n_getcmd(buf, sizeof(buf)) >= 0){
        if (fork() == 0){
            char* argv[MAXARGS];
            int argc = -1;
            n_set(buf, argv, &argc);
            n_runcmd(argv, argc);
        }
        wait(0);
    }
    exit(0);
}