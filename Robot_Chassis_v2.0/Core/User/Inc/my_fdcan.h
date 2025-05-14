/**
 * @file    my_fdcan.h
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
#ifndef MY_FDCAN_H
#define MY_FDCAN_H

#include "main.h"

extern int16_t g_lAngel_Buff[8];
extern int16_t g_rAngel_Buff[8];
extern uint8_t g_key_Buff[34];

void CanInit(void);
uint8_t CanSendMsg(uint32_t _id, uint8_t *_msg, uint32_t _len);

#endif //MY_FDCAN_H
