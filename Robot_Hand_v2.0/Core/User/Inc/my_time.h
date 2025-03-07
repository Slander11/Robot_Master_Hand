/**
 * @file    my_time.h
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
#ifndef MY_TIME_H
#define MY_TIME_H
#include <main.h>

#define TIME_DISABLE    0       /* 软件定时器是否启用 0为启用 */
#define TMR_COUNT	    4		/* 软件定时器的个数 （定时器ID范围 0 - 3) */

/* 软件定时器枚举 */
typedef enum {
    SOFT_TIME1 = 0,
    SOFT_TIME2,
    SOFT_TIME3,
    SOFT_TIME4
} SOFT_WARE_TMR;

/* 工作模式枚举 */
typedef enum {
    TMR_ONCE_MODE = 0, 			/* 一次工作模式 */
    TMR_AUTO_MODE = 1           /* 自动定时工作模式 */
} TMR_MODE_E;

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef struct {
    volatile uint8_t Mode; 		/* 计数器模式，1次性 */
    volatile uint8_t Flag; 		/* 定时到达标志  */
    volatile uint32_t Count; 	/* 计数器 */
    volatile uint32_t PreLoad; 	/* 计数器预装值 */
} SOFT_TMR;

/* 提供给其他C文件调用的函数 */
void InitTimer(void);  								/* 初始化软件定时器变量 */
void StartTimer(uint8_t _id, uint32_t _period); 	/* 单次定时 */
void StartAutoTimer(uint8_t _id, uint32_t _period); /* 自动定时 */
void StopTimer(uint8_t _id); 						/* 停止一个定时器 */
uint8_t CheckTimer(uint8_t _id); 					/* 检测定时器是否超时 */
int32_t GetRunTime(void); 							/* cpu运行时间 */

#endif //MY_TIME_H
