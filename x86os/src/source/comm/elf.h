
#ifndef OS_ELF_H
#define OS_ELF_H

#include "types.h"

// ELF相关数据类型
typedef uint32_t Elf32_Addr;   // 程序运行地址
typedef uint16_t Elf32_Half;   // 中等大小整数
typedef uint32_t Elf32_Off;    // 文件偏移量
typedef uint32_t Elf32_Word;   // 无符号大整数

#pragma pack(1)

// ELF Header
#define EI_NIDENT       16
#define ELF_MAGIC       0x7F

#define ET_EXEC         2   // 可执行文件
#define ET_386          3   // 80386处理器

#define PT_LOAD         1   // 可加载类型

// 32位elf文件头,是描述程序头或节头的头，从全局上给出程序文件的组织结构
typedef struct {
    // 0x7f | 'E'0x45 | 'L'0x4c | 'F'0x46 | 0表示不可识别类型，1表示32elf格式文件，2表示64elf格式文件 |
    // 0表示非法编码格式，1表示小端字节序，2表示大端字节序 | ELF头版本信息，0表示非法版本，1表示当前版本 | 7 ~ 15 字节暂且不用，保留，均初始化为0 
    char e_ident[EI_NIDENT];  
    Elf32_Half e_type;     // 用来指定elf目标文件的类型，0表示未知目标格式，1表示可重定位文件，2表示可执行文件，3表示动态共享目标文件等
    Elf32_Half e_machine;  // 用来描述elf目标文件的体系结构类型
    Elf32_Word e_version;  // 用来表示版本信息
    Elf32_Addr e_entry;    // 用来指明操作系统运行该程序时，将控制权转交给的虚拟地址，即程序的虚拟入口地址
    Elf32_Off e_phoff;     // 用来表明程序表头在文件内的字节偏移量。若没有表头，该值为0
    Elf32_Off e_shoff;     // 用来表明节表头在文件内的字节偏移量。若没有表头，该值为0
    Elf32_Word e_flags;    // 用来指定与处理器相关的标志
    Elf32_Half e_ehsize;   // 用来指明elf header的字节大小
    Elf32_Half e_phentsize;  // 用来指明程序头表中每个条目的字节大小
    Elf32_Half e_phnum;      // 用来指明程序头表中的条目的数量，即段的个数
    Elf32_Half e_shentsize;  // 用来指明节表头中每个条目的大小
    Elf32_Half e_shnum;      // 节头表中条目的数量
    Elf32_Half e_shstrndx;   // 用来指明string name table 在节头表中的索引index
} Elf32_Ehdr;

#define PT_LOAD         1

// 表示程序头表，也就是段头表
typedef struct {
    Elf32_Word p_type;    // 用来指明程序中该段的类型，1表示可加载程序段，2表示动态链接信息，3表示动态加载器名称等
    Elf32_Off p_offset;   // 用来指明本段在内存中的起始偏移字节
    Elf32_Addr p_vaddr;   // 用来指明本段在内存中的起始虚拟地址
    Elf32_Addr p_paddr;   // 仅用于与物理地址相关的系统中
    Elf32_Word p_filesz;  // 用来指明本段在文件中的大小
    Elf32_Word p_memsz;   // 用来指明本段在内存中的大小
    Elf32_Word p_flags;   // 用来指明与本段相关的标志，1表示本段具有可执行权限，2可写，3可读等
    Elf32_Word p_align;   // 用来指明本段在文件和内存中的对齐方式，0/1不对齐，2的幂次数表示按值对齐
} Elf32_Phdr;

#pragma pack()

#endif //OS_ELF_H
