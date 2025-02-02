
#ifndef OS_H
#define OS_H

// 各选择子
#define KERNEL_CODE_SEG         (1 * 8)
#define KERNEL_DATA_SEG         (2 * 8)
#define APP_CODE_SEG            ((3 * 8) | 3)       // 特权级3
#define APP_DATA_SEG            ((4 * 8) | 3)       // 特权级3
#define TASK0_TSS_SEL           (5 * 8)
#define TASK1_TSS_SEL           (6 * 8)
#define SYSCALL_SEL             (7 * 8)
#define TASK0_LDT_SEL           (8 * 8)             // 任务0 LDT
#define TASK1_LDT_SEL           (9 * 8)             // 任务1 LDT

#define TASK_CODE_SEG           (0*0 | 0x4 | 3)         // LDT, 任务代码段
#define TASK_DATA_SEG           (1*8 | 0x4 | 3)         // LDT, 任务代码段

#endif // OS_H
