
#ifndef DEV_H
#define DEV_H

#include "comm/types.h"

#define DEV_NAME_SIZE               32      // 设备名称长度

// 枚举设备类型，用于定义major的值
enum {
    DEV_UNKNOWN = 0,        // 未知类型
    DEV_TTY,                // TTY设备，将屏幕（只写）和键盘（只读）抽象为TTY设备
    DEV_DISK,               // 磁盘设备
};

struct _dev_desc_t;

/**
 * @brief 设备驱动接口
 * 属于某类的特定设备，比如major表示磁盘类型设备，则minor表示计算机上哪一块磁盘
 */
typedef struct _device_t {
    struct _dev_desc_t * desc;      // 设备特性描述符
    int mode;                       // 操作模式
    int minor;                      // 次设备号
    void * data;                    // 设备参数
    int open_count;                 // 打开次数
} device_t;


/**
 * @brief 设备描述结构
 * 描述的是某一类型的设备
 */
typedef struct _dev_desc_t {
    char name[DEV_NAME_SIZE];            // 设备名称
    int  major;                          // 主设备号
    int  (*open)    (device_t * dev);
    int  (*read)    (device_t * dev, int addr, char * buf, int size);
    int  (*write)   (device_t * dev, int addr, char * buf, int size);
    int  (*control) (device_t * dev, int cmd, int arg0, int arg1);
    void (*close)   (device_t * dev);
} dev_desc_t;


// 设备使用统一接口: 只需要知道设备id，就能操作设备
int  dev_open    (int major,  int minor, void * data);
int  dev_read    (int dev_id, int addr,  char * buf, int size);
int  dev_write   (int dev_id, int addr,  char * buf, int size);
int  dev_control (int dev_id, int cmd,   int arg0,   int arg1);
void dev_close   (int dev_id);

#endif // DEV_H
