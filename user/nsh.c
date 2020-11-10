#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int ngetcmd(char *buff, int nbuff); // 读入cmd
void nruncmd(); // 运行读入的cmd

int nfork(void);    // 创建子进程
void errprint(char *s); // 打印错误信息

int
main(void)
{
    static char buff[100];
    int fd;

    // 确保三个fd打开
    while ((fd = open("console", O_RDWR)) >= 0){
        if(fd >=3){
            close(fd);
            break;
        }
    }

    // 读入cmd
    while (ngetcmd(buff, sizeof(buff)) >= 0){
        // 父进程:调用cd指令
        if(buff[0] == 'c' && buff[1] == 'd' && buff[2] == ' '){
            buff[strlen(buff) - 1] = 0;
            if (chdir(buff + 3) < 0)
                fprintf(2, "cannot cd %s\n", buff + 3);
            continue;
        }

        // 子进程
        if (nfork() == 0){
            nruncmd();
        }
        wait(0);
    }
    exit(0);   
}

// 读入cmd
int 
ngetcmd(char *buff, int nbuff)
{
    fprintf(2, "@");
    memset(buff, 0, nbuff);
    gets(buff, nbuff);
    if (buff[0] == 0)
        return -1;
    return 0;    
}

// 创建子进程
int
nfork(void)
{
    int pid;
    pid = fork();
    if(pid == -1)
        errprint("nfork error");
    return pid;
}

// 打印错误信息
void
errprint(char *s)
{
    fprintf(2, "%s\n", s);
    exit(-1);
}