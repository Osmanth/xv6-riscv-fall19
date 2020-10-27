#include "kernel/types.h"
#include "user/user.h"

int main(){
    int parent_fd[2],child_fd[2];
    char buf[64];

    pipe(parent_fd);  // p[0]读出端，p[1]写入端
    pipe(child_fd);
    
    if(fork() == 0){
        // 子进程,将“pong”写入child_fd[1]
        read(parent_fd[0], buf, sizeof(buf));
        printf("%d: received %s\n",getpid(), buf);
        write(child_fd[1], "pong", sizeof("pong"));
        close(child_fd[1]);
    }else{
        // 父进程，将“ping”写入parent_fd[1]
        write(parent_fd[1], "ping", strlen("ping"));
        close(parent_fd[1]);
        read(child_fd[0], buf, sizeof(buf));
        printf("%d: received %s\n",getpid(), buf);
    }
    exit();
    
}

