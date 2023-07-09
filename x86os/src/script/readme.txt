start qemu-system-i386  -m 128M -s -S -serial stdio 

// 对于PrimaryBus的Primary Master Drive，其索引index = 0
// 对于PrimaryBus的Primary Slave  Drive，其索引index = 1
// 课程中使用了disk1和disk2,其中disk1用于保存操作系统代码，不会进行分区，disk2用于分区

-drive file=disk1.vhd,index=0,media=disk,format=raw 
-drive file=disk2.vhd,index=1,media=disk,format=raw 

-d pcall,page,mmu,cpu_reset,guest_errors,page,trace:ps2_keyboard_set_translation
