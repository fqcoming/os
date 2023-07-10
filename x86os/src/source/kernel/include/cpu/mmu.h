
#ifndef MMU_H
#define MMU_H

// 内存管理单元: MMU

#include "comm/types.h"
#include "comm/cpu_instr.h"

#define PDE_CNT             1024      // 页目录表项数量
#define PTE_CNT             1024      // 页表项数量
#define PTE_P              (1 << 0)   // 若为1表示该页存在于物理内存中，若为0表示该表不在物理内存中，访问时会触发pagefault
#define PTE_W              (1 << 1)   // 若为1表示可读可写，若为0表示可读不可写
#define PDE_P              (1 << 0)
#define PTE_U              (1 << 2)   // 若为1表示User级任意级别特权的程序都可以访问该页，若为0表示Supervisor特征级3不能访问
#define PDE_U              (1 << 2)

#pragma pack(1)

/**
 * @brief Page-Table Entry
 */
typedef union _pde_t {
    uint32_t v;
    struct {
        uint32_t present : 1;                   // 0 (P) Present; must be 1 to map a 4-KByte page
        uint32_t write_disable : 1;             // 1 (R/W) Read/write, if 0, writes may not be allowe
        uint32_t user_mode_acc : 1;             // 2 (U/S) if 0, user-mode accesses are not allowed t
        uint32_t write_through : 1;             // 3 (PWT) Page-level write-through
        uint32_t cache_disable : 1;             // 4 (PCD) Page-level cache disable
        uint32_t accessed : 1;                  // 5 (A) Accessed
        uint32_t : 1;                           // 6 Ignored;
        uint32_t ps : 1;                        // 7 (PS)
        uint32_t : 4;                           // 11:8 Ignored
        uint32_t phy_pt_addr : 20;              // 高20位page table物理地址
    };
} pde_t;

/**
 * @brief Page-Table Entry
 */
typedef union _pte_t {
    uint32_t v;
    struct {
        uint32_t present : 1;                   // 0 (P) Present; must be 1 to map a 4-KByte page
        uint32_t write_disable : 1;             // 1 (R/W) Read/write, if 0, writes may not be allowe
        uint32_t user_mode_acc : 1;             // 2 (U/S) if 0, user-mode accesses are not allowed t
        uint32_t write_through : 1;             // 3 (PWT) Page-level write-through
        uint32_t cache_disable : 1;             // 4 (PCD) Page-level cache disable
        uint32_t accessed : 1;                  // 5 (A) Accessed;
        uint32_t dirty : 1;                     // 6 (D) Dirty
        uint32_t pat : 1;                       // 7 PAT
        uint32_t global : 1;                    // 8 (G) Global
        uint32_t : 3;                           // Ignored
        uint32_t phy_page_addr : 20;            // 高20位物理地址
    };
} pte_t;

#pragma pack()

/**
 * @brief 返回vaddr在页目录中的索引
 * param1 vaddr: 表示虚拟内存地址
 * return : 返回虚拟内存地址在页目录表中的索引下标
 */
static inline uint32_t pde_index (uint32_t vaddr) {
    int index = (vaddr >> 22); // 只取高10位
    return index;
}

/**
 * @brief 获取pde中地址
 */
static inline uint32_t pde_paddr (pde_t * pde) {
    return pde->phy_pt_addr << 12;
}

/**
 * @brief 返回vaddr在页表中的索引
 */
static inline int pte_index (uint32_t vaddr) {
    return (vaddr >> 12) & 0x3FF;   // 取中间10位
}

/**
 * @brief 获取pte中的物理地址
 */
static inline uint32_t pte_paddr (pte_t * pte) {
    return pte->phy_page_addr << 12;
}

/**
 * @brief 获取pte中的权限位
 */
static inline uint32_t get_pte_perm (pte_t * pte) {
    return (pte->v & 0x3FF);
}

/**
 * @brief 重新加载整个页表
 * @param vaddr 页表的虚拟地址
 */
static inline void mmu_set_page_dir (uint32_t paddr) {
    // 将虚拟地址转换为物理地址
    write_cr3(paddr);
}

#endif // MMU_H
