#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int findstat(char *filename) //从stat里找到表示状态的符号
{
  FILE *fp;
  int c;
  fp = fopen(filename, "r");
  while ((c = fgetc(fp)) != EOF)
  {
    // printf("%c\n", c);
    if (c == ')') //右括号后的第二个字符// 225 (scsi_eh_2) S 2 0 0 0 -1 2129984 0 0 0 0 0 0 0 0 20 0 1 0 584 0 0 18446744073709551615 0 0 0 0 0 0 0 2147483647 0 0 0 0 17 3 0 0 0 0 0 0 0 0 0 0 0 0 0

    {
      c = fgetc(fp);
      c = fgetc(fp);
      printf("%c\t", c);
      fclose(fp);
      return 0;
    }
  }
  return 1;
}
int isnull(char *filename) //判断文件内容是否为空
{
  FILE *fp;
  char ch;
  if ((fp = fopen(filename, "r")) == NULL)
  {
    printf("Error!\n");
    exit(0);
  }
  ch = fgetc(fp);
  if (ch == EOF)
  {
    fclose(fp);
    // printf("文件为空\n");
    return 1;
  }
  else
  {
    fclose(fp);
    // printf("文件不为空\n");
    return 0;
  }
}

void mystrc(char *s, char *t)
{
  char *result;
  // printf("%p\n", s);
  // make the pointer to the end of str
  while (*t != 0)
    t++;
  *t = '/';
  t++;
  while (*s != 0)
  {
    *t = *s;
    t++;
    s++;
  }
}

void readtext(char *filename) //普通的读所有内容功能
{
  FILE *fileStream;

  //先初始化文件指针

  fileStream = fopen(filename, "rb");

  //先获取文件大小，以分配文本流空间

  // fread(stringPointer, fileSize, 1, fileStream);
  size_t len = 0;
  char *contents = NULL;
  while (getline(&contents, &len, fileStream) != -1)
  {
    if (contents[0] != '\0')
    {
      printf("%s\t", contents);
    }
  }
  //关闭文件
  // printf("文件%s的内容是:%s", filename, stringPointer);
  fclose(fileStream);
}

void ps(char *dir, int depth)
{
  DIR *dp;              // DIR结构指针
  struct dirent *entry; // dirent结构指针
  // for (int i = 0; i < MAX_BUF_SIZE; i++)
  // {
  //   pid[i] = (char *)malloc(6 * sizeof(char));
  // }
  if ((dp = opendir(dir)) == NULL) //打开文件失败
  {
    fprintf(stderr, "can not open this dir:%s\n", dir); //打印到标准错误，也就是在屏幕上输出错误信息
    return;                                             //退出
  }

  chdir(dir);                           //进入目录
  while ((entry = readdir(dp)) != NULL) //循环遍历
  {
    if (entry->d_type == DT_DIR) //如果是目录
    {
      if (strcmp(".", entry->d_name) == 0 || strcmp("..", entry->d_name) == 0) //因为已经遍历过. ..两个目录了排除这两个目录
      {
        continue;
      }
      if (entry->d_name[0] - '0' >= 0 && entry->d_name[0] - '0' <= 9) //开头是数字，那就是进程的文件了
      {

        // printf("dir_cmdline:%s\n", dir_cmdline);
        // *dir_cmdline = *dir;
        // printf("dir_cmdline111:%s\n", dir_cmdline);
        // *pid[index] = *entry->d_name;
        // index++;
        // printf("PID pid[index] :%s\n", pid[index]);
        printf("pid:%05s\t", entry->d_name); // 右对齐
        const char *dir1 = dir;              // char *转化为const char *
                                             // printf("dir:%s\n", dir);
                                             //  const char *entry = entry->d_name;
        // printf("129没问题\n");
        char dir_state[20] = {0}; //随便给个大致长度吧 malloc不能用strcat
        strcat(dir_state, dir1);
        strcat(dir_state, "/");
        strcat(dir_state, entry->d_name);
        strcat(dir_state, "/stat");

        char dir_cmdline[20] = {0};
        strcat(dir_cmdline, dir1);
        strcat(dir_cmdline, "/");
        strcat(dir_cmdline, entry->d_name);
        strcat(dir_cmdline, "/cmdline");

        findstat(dir_state);

        // printf("dir_cmdline:%s\n", dir_cmdline);
        if (isnull(dir_cmdline)) //判断文件内容是否为空
        {
          // printf("没有cmdline，那就读comm喽\n");
          char dir_comm[20] = {0};
          strcat(dir_comm, dir1);
          strcat(dir_comm, "/");
          strcat(dir_comm, entry->d_name);
          strcat(dir_comm, "/comm");
          // printf("这是comm");
          readtext(dir_comm);
        }
        else
        {
          readtext(dir_cmdline);
        }
        // findstat(dir_state);
        printf("\n"); //最后加个换行
        // mystrc(entry->d_name, dir);

        // strcat(dir_copy, "/comm");
        // // readtext()
        // // //  chdir(dir + *entry->d_name);
        // printf("dir_copy:%s", dir_copy);
      }

      // for (int i = 0; i < MAX_BUF_SIZE; i++)
      // {
      //   free(pid[i]);
      // }
      // free(pid);
      // printfdir(entry->d_name, depth + 4); //递归调用
    }
    else //不是目录就是文件
    {
      // printf("%*s%s\n", depth, " ", entry->d_name);
    }
  }
  chdir("..");  //返回上级目录
  closedir(dp); //关闭当前目录流
}

int main(int argc, char *argv[])
{
  char *topdir = ".";
  if (argc >= 2)
  {
    topdir = argv[1];
  }
  // printf("asdfasdf,%s", topdir);
  // printf("directory scan of %s:\n", topdir);
  ps("/proc", 0);
  printf("done\n");
  exit(0);
}
