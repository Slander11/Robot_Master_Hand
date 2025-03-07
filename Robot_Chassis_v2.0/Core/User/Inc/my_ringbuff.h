/**
 * @file    my_ringbuff.h
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
#ifndef MY_RINGBUFF_H
#define MY_RINGBUFF_H

#include "main.h"

#define BUFF_SIZE 256   // 缓存大小

enum
{
    RING_FALSE = 0,
    RING_TRUE,
};

enum
{
    RING_ERR = 0,
    RING_OK,
};

typedef struct _ringbuffer_t
{
    volatile uint16_t head;     /* 当前读位置 */
    volatile uint16_t tail;     /* 当前写位置 */
    uint16_t size;              /* ringbuffer大小 */
    uint8_t *buffer;            /* 指向ringbuffer */
} RingBuff_t;

extern uint8_t Usb_Buff[BUFF_SIZE];
extern uint8_t LeftArm_Buff[BUFF_SIZE];
extern uint8_t RightArm_Buff[BUFF_SIZE];
extern uint8_t Boot_Buff[BUFF_SIZE];

extern RingBuff_t BootLoader_RingBuff_t;
extern RingBuff_t Usb_RingBuff_t;
extern RingBuff_t LeftArm_RingBuff_t;
extern RingBuff_t RightArm_RingBuff_t;

void RingBuffInit(RingBuff_t *_pRingBuff, uint8_t *_pBuff, uint16_t _buf_size);
int RingBuffEmpty(RingBuff_t *_pRingBuff);
int RingBuffReadNByte(RingBuff_t *_pRingBuff, uint8_t *_pData, uint16_t _size);
int RingBuffWriteNByte(RingBuff_t *_pRingBuff, uint8_t *_pData, uint16_t _size);
uint16_t RingBuffGetLen(RingBuff_t *_pRingBuff);
int RingBuffGetNByte(RingBuff_t *_pRingBuff, uint8_t *_pData, uint16_t _size);
int RingBuffRemoveNByte(RingBuff_t *_pRingBuff, uint16_t _size);
int RingBuffRemoveOneByte(RingBuff_t *_pRingBuff);
uint8_t RingBuffGetHeadItem(RingBuff_t *_pRingBuff);
void RingBuffClear(RingBuff_t *_pRingBuff);
uint8_t RingBuffGetHeadItem(RingBuff_t *_pRingBuff);
uint8_t RingBuffGetIndexItem(RingBuff_t *_pRingBuff, uint16_t _index);
uint8_t RingBuffReWriteIndexItem(RingBuff_t *_pRingBuff, uint16_t _index, uint8_t _data);
#endif //MY_RINGBUFF_H
