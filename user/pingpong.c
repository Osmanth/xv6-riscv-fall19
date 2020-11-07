#include "kernel/types.h"
#include "user/user.h"

int main(){
    int parent_fd[2],child_fd[2];
    char buf[64];   // 读出缓存
    
    // 创建两个管道（单向） p[0]读出端，p[1]写入端
    pipe(parent_fd);
    pipe(child_fd);
    
    if(fork() == 0){
        // 子进程,将“pong”写入child_fd[1]
        read(parent_fd[0], buf, sizeof(buf));
        printf("%d: received %s\n",getpid(), buf);
        write(child_fd[1], "pong", sizeof("pong"));
        close(child_fd[1]);
    }else{
        // 父进程，将“ping”写入parent_fd[1]
        write(parent_fd[1], "ping", sizeof("ping"));
        close(parent_fd[1]);
        read(child_fd[0], buf, sizeof(buf));
        printf("%d: received %s\n",getpid(), buf);
    }
    exit();
    
}

