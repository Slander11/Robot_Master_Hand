/**
 * @file    my_globe.h
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
#ifndef MY_GLOBE_H
#define MY_GLOBE_H

#include "stdint.h"

#define min(X, Y) ((X) < (Y) ? (X) : (Y))
#define max(X, Y) ((X) < (Y) ? (Y) : (X))
// 位操作
#define GetBit(x, y) ((x) >> (y) & 1) /* 获取将x的第y位（0或1）*/
#define SetBit(x, y) x |= (1 << y)    /* 将X的第Y位置1 */
#define ClrBit(x, y) x &= ~(1 << y)   /* 将X的第Y位清0 */

typedef union HexToFloat
{
    int16_t i;
    float f;
} HexToFloat_value;

typedef union BitToHex
{
    uint8_t bit[2];
    uint16_t hex;
} BitToHex_value;

typedef union BitToFloat
{
    uint8_t bit[4];
    float float_r;
} BitToFloat_value;

typedef union BitToUint32
{
    uint8_t bit[4];
    uint32_t uint32;
} BitToUint32_value;

#endif //MY_GLOBE_H
