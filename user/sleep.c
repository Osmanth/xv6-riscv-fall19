#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Wrong input\n");
        exit();
    }
    
    sleep(atoi(argv[1]));   //使用atio将输入的字符串转换成整数
    exit();
}