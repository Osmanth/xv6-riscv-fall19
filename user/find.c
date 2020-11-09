#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, char *re);

int main(int argc, char *argv[])
{
  if(argc < 3){
    fprintf(2, "usage: find <path> <expression>\n");
    exit();
  }
  find(argv[1], argv[2]);
  exit();
}

void find(char *path, char *re) 
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  // 打开文件
  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }
  // fstat用于判断fd的type
  if(fstat(fd, &st) < 0 || T_DIR != st.type){
    fprintf(2, "find: the first arg must be dir path\n");
    close(fd);
    return;
  }
  // 进入子目录
  while(read(fd, &de, sizeof(de)) == sizeof(de)) {
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    if(de.inum == 0)
      continue;
    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;
    if(stat(buf, &st) < 0){
      printf("find: cannot stat %s\n", buf);
      continue;
    }
    // 若为文件，与文件名匹配
    // 若为目录，循环read()，得到子文件，拼接路径后递归
    switch(st.type) {
      case T_FILE:
        if (strcmp(re, de.name) == 0)
          printf("%s\n", buf);
        break;
      case T_DIR:
        // 递归
        if (strcmp(de.name, ".") != 0 && strcmp(de.name, "..") != 0)
          find(buf, re);
        break;
      }
  }
  close(fd);
}

