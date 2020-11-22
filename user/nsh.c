// MyShell.
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"
#define MAXARGS 10
#define MAXLEN 30
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

// 
void n_set(char *cmd, char* argv[],int* argc)
{
    char needJump[] = " \t\r\n\v";
    char args[MAXARGS][MAXLEN];
    // 让argv的每一个元素都指向args的每一行
    for(int m=0; m < MAXARGS; m++){
        argv[m] = &args[m][0];
    }

    int i = 0; // 表示第i个word
    int j = 0;
    for (; cmd[j] != '\n' && cmd[j] != '\0'; j++)
    {
        // 跳过空格
        while (strchr(needJump,cmd[j]))
            j++;
        argv[i++]=cmd+j;
        // 只要不是空格，就j++,找到下一个空格
        while (strchr(needJump,cmd[j])==0)
            j++;

        cmd[j]='\0';
    }
    argv[i]=0;
    *argc=i;
}

void n_pipe(char*argv[], int argc);

void n_runcmd(char*argv[],int argc)
{
    for(int i=1;i<argc;i++){
        if(!strcmp(argv[i],"|")){
            // 如果遇到 | 即pipe，至少说明后面还有一个命令要执行
            n_pipe(argv,argc);
        }
    }
    // 此时是仅处理一个命令：现在判断argv[1]开始，后面有没有> 
    for(int i=1;i<argc;i++){
        // 如果遇到 > ，说明需要执行输出重定向，首先需要关闭stdout
        if(!strcmp(argv[i],">")){
            close(1);
            // 此时需要把输出重定向到后面给出的文件名对应的文件里
            // 当然如果>是最后一个，那就会error，不过暂时先不考虑
            open(argv[i+1],O_CREATE|O_WRONLY);
            argv[i]=0;
        }
        if(!strcmp(argv[i],"<")){
            // 如果遇到< ,需要执行输入重定向，关闭stdin
            close(0);
            open(argv[i+1],O_RDONLY);
            argv[i]=0;
        }
    }
    exec(argv[0], argv);
}

// 执行管道
void n_pipe(char*argv[], int argc){
    int i=0;
    int fd[2];
    pipe(fd);
    // 把cmd中的‘|’换成空字符‘\0’
    for(; i<argc; i++){
        if(!strcmp(argv[i],"|")){
            argv[i]=0;
            break;
        }
    }
    // 先考虑最简单的情况：cat file | wc
    if(fork()==0){
        // 子进程 执行左边的命令 把自己的标准输出关闭
        close(1);
        dup(fd[1]);
        close(fd[0]);
        close(fd[1]);
        
        n_runcmd(argv,i);
    }
    else{
        // 父进程 执行右边的命令 把自己的标准输入关闭
        close(0);
        dup(fd[0]);
        close(fd[0]);
        close(fd[1]);
        
        n_runcmd(argv+i+1,argc-i-1);
    }
}

int main()
{
    char buf[MAXIN];
    while (n_getcmd(buf, sizeof(buf)) >= 0){
        if (fork() == 0){
            char* argv[MAXARGS];
            int argc=-1;
            n_set(buf, argv,&argc);
            n_runcmd(argv,argc);
        }
        wait(0);
    }
    exit(0);
}