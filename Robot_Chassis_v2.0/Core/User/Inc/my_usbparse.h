/**
 * @file    my_usbparse.h
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
#ifndef MY_USBPARSE_H
#define MY_USBPARSE_H

#include "stdint-gcc.h"
#include "main.h"

/* 串口数据转换输出 */
extern uint8_t g_OutManner;
extern uint8_t g_ParseResults;
extern uint8_t g_AllUpgrade;
extern uint8_t g_Shakehands;
extern uint8_t g_UpgradeBegin;
extern uint8_t g_ReceiveNg;
extern uint8_t g_EndData;
// 控制指令解析状态机
enum {
    no_enough = 0, // 数据有误
    no_len,        // 数据长度不够
    enough,        // 数据无误
    no_begin,      // 不是帧头
    no_end,        // 不是帧尾
    data_error,    // 数据错误
    crc_error,     // crc错误
    hand_shake,    // 握手
    end_data,      // 数据接收结束
};



#define SET_ZERO 0x01
#define SET_DIRECTION 0x02
#define SET_FLITER 0x03
#define CHANGE_DATA 0x04
#define FLASH_STORE 0x0F
#define UPGRADEALL 0XDD
#define UPGRADE   0XBB
#define UPGRADESIGN 0X0B
#define UPGRADESIGNALL 0X0D
#define REBOOT 0xFF

void ReceiveDisassembly(void);
void RingBuffMoveRight();
void USBReceiveDataProcess(uint8_t *pdata);
void ChangeData(void);


#endif //MY_USBPARSE_H
