
#ifndef LIB_SYSCALL_H
#define LIB_SYSCALL_H

#include "core/syscall.h"
#include "os_cfg.h"
#include "fs/file.h"
#include "dev/tty.h"

#include <sys/stat.h>
typedef struct _syscall_args_t {
    int id;    // 指定调用的内核函数的id号, 其id号就是sys_table表中的索引下标
    int arg0;
    int arg1;
    int arg2;
    int arg3;
} syscall_args_t;

int msleep (int ms);
int fork(void);
int getpid(void);
int yield (void);
int execve(const char *name, char * const *argv, char * const *env);
int print_msg(char * fmt, int arg);
int wait(int* status);
void _exit(int status);

int open(const char *name, int flags, ...);
int read(int file, char *ptr, int len);
int write(int file, char *ptr, int len);
int close(int file);
int lseek(int file, int ptr, int dir);
int isatty(int file);
int fstat(int file, struct stat *st);
void * sbrk(ptrdiff_t incr);
int dup (int file);
int ioctl(int fd, int cmd, int arg0, int arg1);

struct dirent {
   int index;         // 在目录中的偏移
   int type;            // 文件或目录的类型
   char name [255];       // 目录或目录的名称
   int size;            // 文件大小
};

typedef struct _DIR {
    int index;               // 当前遍历的索引
    struct dirent dirent;    // 目录文件中的目录项
} DIR;


DIR * opendir(const char * name);   // name: 需要打开的目录的路径
struct dirent* readdir(DIR* dir);
int closedir(DIR *dir);


int unlink(const char *pathname);

#endif //LIB_SYSCALL_H
