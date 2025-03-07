/**
 * @file    my_vofa.h
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
#ifndef MY_VOFA_H
#define MY_VOFA_H

#include "main.h"

/* 串口接收数组 */
extern uint8_t receive_buffer1[256];

#define CH_COUNT (64)

typedef struct Frame {
    float fdata[CH_COUNT];
    unsigned char tail[4]; //{0x00, 0x00, 0x80, 0x7f};
} Frame_t;

// --------------------串口通信相关结构体
static inline void FRAME_Init(Frame_t *f) {
    f->tail[0] = 0x00;
    f->tail[1] = 0x00;
    f->tail[2] = 0x80;
    f->tail[3] = 0x7f;
}

extern Frame_t BoaruUploadBuff;

void BOARDUploadStart(void);

#endif //MY_VOFA_H
