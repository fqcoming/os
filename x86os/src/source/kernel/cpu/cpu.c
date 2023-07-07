
#include "comm/cpu_instr.h"
#include "cpu/cpu.h"
#include "cpu/irq.h"
#include "os_cfg.h"
#include "ipc/mutex.h"
#include "core/syscall.h"

static segment_desc_t gdt_table[GDT_TABLE_SIZE];
static mutex_t mutex;

/**
 * 设置段描述符
 * 初始化GDT表中段描述符项
 * selector: 设置哪一个GDT表项，即GDT表项的偏移，以字节为单位，除以8之后即表项在GDT表的下标
 * base: 段描述符中表示某个段的基地址
 * limit: 段的界限，段的扩展方向只有上下两种，界限表示段内偏移的最大值，单位为 B 字节
 * 结合《真相还原》151页理解
 */
void segment_desc_set(int selector, uint32_t base, uint32_t limit, uint16_t attr) {
    segment_desc_t * desc = gdt_table + (selector >> 3);

	// 如果界限比较长，将长度单位换成4KB
	if (limit > 0xfffff) {  // 如果界限值大于1MB,那么段界限单位要使用4kb的单位粒度
		attr |= 0x8000;     // 设置 段描述符 G 字段的值，段界限只是个单位量，它的单位要么是字节，要么是4kb
                            // 如果 G = 1 表示段界限粒度大小为4kb，= 0 则表示段界限粒度为1字节
                            // 粒度为1字节则只能表示1MB段内偏移，如果为4kb则可以表示4GB偏移
		limit /= 0x1000;    // 当粒度换成4kb后，要将limit原来单位是1B换成4kb，故除以4kb
	}
	desc->limit15_0 = limit & 0xffff;  // 设置段界限 15 ~ 0
	desc->base15_0 = base & 0xffff;   // 设置段基址 15 ~ 0
	desc->base23_16 = (base >> 16) & 0xff;  // 设置段基址 23 ~ 16
	desc->attr = attr | (((limit >> 16) & 0xf) << 8);  // 设置段界限 19 ~ 16
	desc->base31_24 = (base >> 24) & 0xff;   // 设置段基址 31 ~ 24
}

/**
 * 设置门描述符
 */
void gate_desc_set(gate_desc_t * desc, uint16_t selector, uint32_t offset, uint16_t attr) {
	desc->offset15_0 = offset & 0xffff;
	desc->selector = selector;
	desc->attr = attr;
	desc->offset31_16 = (offset >> 16) & 0xffff;
}

void gdt_free_sel (int sel) {
    mutex_lock(&mutex);
    gdt_table[sel / sizeof(segment_desc_t)].attr = 0;
    mutex_unlock(&mutex);
}

/**
 * 分配一个GDT推荐表符
 */
int gdt_alloc_desc (void) {
    int i;

    // 跳过第0项
    mutex_lock(&mutex);
    for (i = 1; i < GDT_TABLE_SIZE; i++) {
        segment_desc_t * desc = gdt_table + i;
        if (desc->attr == 0) {
            desc->attr = SEG_P_PRESENT;     // 标记为占用状态
            break;
        }
    }
    mutex_unlock(&mutex);

    return i >= GDT_TABLE_SIZE ? -1 : i * sizeof(segment_desc_t);;
}

/**
 * 初始化GDT
 */
void init_gdt(void) {
	// 全部清空
    // 将GDT表中所有项都设置为0
    for (int i = 0; i < GDT_TABLE_SIZE; i++) {
        segment_desc_set(i << 3, 0, 0, 0);  // 每个GDT表占64位，8个字节，右移3位表示乘以8，即表项偏移字节数
    }

    // 采用平坦模型，分为代码段和数据段
    // 第0个表项保留不能使用
    // 数据段
    segment_desc_set(KERNEL_SELECTOR_DS, 0x00000000, 0xFFFFFFFF,
                     SEG_P_PRESENT | SEG_DPL0 | SEG_S_NORMAL | SEG_TYPE_DATA // 0010 表示可读写
                     | SEG_TYPE_RW | SEG_D | SEG_G);

    // 结合《真相还原》151页学习
    // 只能用非一致代码段，以便通过调用门更改当前任务的CPL执行关键的资源访问操作
    // 基地址为0x00000000，段界限0xFFFFFFFF字节
    segment_desc_set(KERNEL_SELECTOR_CS, 0x00000000, 0xFFFFFFFF,
                     // 属性字段，共8个属性需要设置
                     SEG_P_PRESENT |    // 段是否存在，如果存在于内存中，P为1，否则P为0，这里设置为存在
                     SEG_DPL0 |         // 内存段的特权级，由于这是内核段，特权级应该为0
                     SEG_S_NORMAL |     // 用来指示当前描述符是否是系统段，为0表示系统段，为1表示非系统段
                     SEG_TYPE_CODE |    // 用于表示内存段或门的子类型，由于S位为1非系统段，1000表示只执行代码段
                     SEG_TYPE_RW |      // 1000 | 0010 => 1010 标志该段为可执行可读代码段
                     SEG_D |            // 对于代码段来说，此位是D位，D为1表示指令中有效地址及操作数是32位
                     SEG_G              // 段界限单位，1表示4kb，0表示1B
                    );  // 对于属性段L默认0表示32位代码段，为1表示64位代码段，AVL没有明确的含义，用户可用于自定义用途

    // 调用门
    gate_desc_set((gate_desc_t *)(gdt_table + (SELECTOR_SYSCALL >> 3)),  // 调用门段描述符在GDT表中的地址
            KERNEL_SELECTOR_CS,                                          // 被调用例程所在代码段的描述符选择子
            (uint32_t)exception_handler_syscall,                         // 被调用例程在目标代码段内的偏移量
            GATE_P_PRESENT | GATE_DPL3 | GATE_TYPE_SYSCALL | SYSCALL_PARAM_COUNT);  // 

    // 加载gdt
    lgdt((uint32_t)gdt_table, sizeof(gdt_table));
}

/**
 * 切换至TSS，即跳转实现任务切换
 */
void switch_to_tss (uint32_t tss_selector) {
    far_jump(tss_selector, 0);
}

/**
 * CPU初始化
 */
void cpu_init (void) {
    mutex_init(&mutex);
    init_gdt();  // 对GDT表进行初始化
}
