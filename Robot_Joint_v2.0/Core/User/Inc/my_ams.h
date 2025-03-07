/**
 * @file    my_ams.h
 * @brief   简要描述本头文件的功能
 * @author  yanyan6.liu
 * @date    2025/3/6
 * @version 1.0.0
 * @license 许可证 (如 MIT, BSD, 或公司协议)
 * 
 * 详细描述（可选）：
 * 这里是更详细的说明，可以包含模块功能、设计思路、依赖关系等。
 * 
 * @note    注意事项或待办项（可选）
 */
#ifndef MY_AMS_H
#define MY_AMS_H

#include "main.h"

/*读取数据命令*/
#define READ_ANGLECOM 0xFFFF
#define READ_NOP      0xC000
#define READ_ERRFL    0x4001
#define READ_AGC      0x7FFD

#define MPS_ANGLECOM  0x0000

extern uint16_t g_AmsError;
extern uint16_t g_AmsOriginAngle;
extern uint16_t g_AmsDataprocessAngle;

uint16_t TransReceive16(uint16_t trans);
void ReadAmsSensor(uint16_t *sensor);
unsigned int EvenCheck(unsigned int v);
uint16_t SendDataProcess(void);
#endif //MY_AMS_H
