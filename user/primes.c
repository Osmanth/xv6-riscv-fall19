#include "kernel/types.h"
#include "user/user.h"

void primes();

int main(){
    int fd[2];

    pipe(fd);
    if(fork() == 0){
        // 子进程
        close(0);
        dup(fd[0]);
        close(fd[1]);
        close(fd[0]);
        primes();
    }else{
        // 父进程
        close(1);
        dup(fd[1]);
        close(fd[1]);
        close(fd[0]);
        for (int i = 2; i <= 35; i++){
            write(1, &i, sizeof(int));
        }
        close(1);
        wait();        
    }
    exit();
}

void primes(){
    int n, p, len;
    int fd[2];

    if ((len = read(0, &n, sizeof(int))) <= 0 || n <= 0){
        close(1);
        exit();
    }
    // 输出prime
    printf("prime %d\n", n);

    pipe(fd);
    if(fork() == 0){
        // 子进程
        close(0);
        dup(fd[0]);
        close(fd[1]);
        close(fd[0]);
        primes();
    } else {
        // 父进程
        close(1);
        dup(fd[1]);
        close(fd[1]);
        close(fd[0]);
        while ((len = read(0, &p, sizeof(int))) > 0 && p > 0){
        if (p % n != 0) {
            write(1, &p, sizeof(int));
        }
    }
    if (len <= 0 || p <= 0) {
      close(1);
      exit();
    }
  }
}