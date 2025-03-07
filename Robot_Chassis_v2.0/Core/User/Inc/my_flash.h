/**
 * @file    my_flash.h
 * @brief   简要描述本头文件的功能
 * @author  yanyan6.liu
 * @date    2025/3/5
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
/* FLASH起始地址 */
#define STM32_FLASH_SIZE        0X20000                     /* STM32G431CBT6 FLASH 总大小 */
#define STM32_FLASH_BASE        0x08000000                  /* STM32 FLASH 起始地址 */

#define FLASH_WAITETIME         50000                       /* FLASH等待超时时间 */
#define PAGE_SIZE               0x800                       /* 页大小 */

/* FLASH 扇区的起始地址,1个bank,每个bank 2kb */
/* BANK1 */
#define BANK1_START_ADDR        ((uint32_t)0x08000000)                      /* Bank1页0起始地址 */
#define BANK1_FLASH_PAGE(n)     (BANK1_START_ADDR + ((n) * PAGE_SIZE))      /* 定义Bank1的页面地址 */


uint32_t FlashReadWord(uint32_t _faddr);
void FlashRead(uint32_t _raddr, uint32_t *_pbuf, uint32_t _length);
uint8_t GetFlashSector(uint32_t _addr);
void FlashWrite(uint32_t _waddr, uint32_t *_pbuf, uint32_t _length);
void FlashNoEraseWrite(uint32_t _waddr, uint32_t *_pbuf, uint32_t _length);
uint8_t FalshErasePage(uint32_t _waddr);


#endif //MY_FLASH_H
