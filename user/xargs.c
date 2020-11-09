#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  int i, j, pid, len, first_blank;
  char args[MAXARG][32];
  char *p[MAXARG];
  char buf;

  if(argc < 2) {
    fprintf(2, "usage: xargs <cmd> ...\n");
    exit();
  }
  while (1)
  {
    i = 0;
    first_blank = 0;
    memset(args, 0, MAXARG * 32);

    for(j = 1; j < argc; j++) 
      strcpy(args[i++], argv[j]);

    j = 0;

    while (i < MAXARG-1) {
      // 退出：CTRL+D 
      if ((len = read(0, &buf, 1)) <= 0) {
        wait(); 
        exit();
      }
      if (buf == '\n') 
        break;
      if (buf == ' ') {
        if (first_blank) {
          i++;
          j = 0;
          first_blank = 0;
        }
        continue;
      }
      args[i][j++] = buf;
      first_blank = 1;
    }
    for (i = 0; i < MAXARG-1; i++) 
      p[i] = args[i];

    p[MAXARG-1] = 0;
    // 调用指令
    if ((pid = fork()) == 0) {
      exec(argv[1], p);
      exit();
    }
  }
  exit();
}