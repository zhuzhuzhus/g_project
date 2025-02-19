#ifndef __UHF_RFID_H__
#define __UHF_RFID_H__

// EPC C1 G2 (ISO18000-6C) 命令宏定义
#define CMD_INVENTORY            0x01  // 询查标签
#define CMD_READ                 0x02  // 读数据
#define CMD_WRITE                0x03  // 写数据
#define CMD_WRITE_EPC            0x04  // 写 EPC 号
#define CMD_KILL                 0x05  // 销毁标签
#define CMD_SET_ACCESS_PASSWORD  0x06  // 设定存储区读写保护状态
#define CMD_BLOCK_ERASE          0x07  // 块擦除
#define CMD_SET_READ_PROTECT_EPC 0x08  // 根据 EPC 号设定读保护设置
#define CMD_SET_READ_PROTECT     0x09  // 不需要 EPC 号读保护设定
#define CMD_UNLOCK_READ_PROTECT  0x0A  // 解锁读保护
#define CMD_CHECK_READ_PROTECT   0x0B  // 测试标签是否被设置读保护
#define CMD_EAS_ALARM_SET        0x0C  // EAS 报警设置
#define CMD_EAS_ALARM_DETECT     0x0D  // EAS 报警探测
#define CMD_USER_MEMORY_LOCK     0x0E  // user 区块锁
#define CMD_SINGLE_TAG_INVENTORY 0x0F  // 询查单标签
#define CMD_BLOCK_WRITE          0x10  // 块写




#endif