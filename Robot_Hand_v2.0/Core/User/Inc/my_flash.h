/**
 * @file    my_flash.h
 * @brief   简要描述本头文件的功能
 * @author  yanyan6.liu
 * @date    2025/3/4
 * @version 1.0.0
 * @license 许可证 (如 MIT, BSD, 或公司协议)
 * 
 * 详细描述（可选）：
 * 这里是更详细的说明，可以包含模块功能、设计思路、依赖关系等。
 * 
 * @note    注意事项或待办项（可选）
 */
#ifndef MY_FLASH_H
#define MY_FLASH_H

#include "main.h"

#define STM32FLASH_BASE FLASH_BASE                      /* FLASH起始地址 */
#define STM32FLASH_PAGE_SIZE FLASH_PAGE_SIZE            /* FLASH页大小：2K */
#define STM32FLASH_DEVICE_ID ((uint32_t)0x1FFF7590)     /* FLASH唯一设备ID地址 */
#define WRITE_START_ADDR ((uint32_t)0x08005000)         /* FLASH启动器参数 */
#define FLASH_WAITETIME  50000                          /* FLASH等待超时时间 */

#define STM32_UPGRADE       0xBBBBBBBB                  /* 32升级标志 */
#define STM32_UPGARDE_FIN   0xAAAAAAAA                  /* 32升级完成 */

//---------------内部flash存储相关变量-------------
typedef struct _intern_flash {
    uint32_t upgrade;       /* 升级标志 */
    uint32_t version;       /* 版本名称 */
    uint8_t is_enable;      /* 使能按键 */
    uint8_t can_id;         /* 设备id号 */
    uint8_t freq_can;       /* 发送频率 */
    uint8_t direction;      /* 旋转方向 */
    uint16_t zero;          /* 零点 */
    uint8_t iap_id;         /* iap升级id号 */
} INTERN_FLASH;

extern INTERN_FLASH flash_param;

void ReadFlashSpecifyLen(uint32_t _raddr, uint32_t *_pbuf, uint32_t _length);
void WriteFlashSpecifyLen(uint32_t _raddr, uint32_t *_pbuf, uint32_t _length);
void InitFlash(void);
void WriteFlashDefault(uint8_t mode);
void ReadChipId(uint32_t _chipId);
void UpgradeFlashExamine();

#endif //MY_FLASH_H
