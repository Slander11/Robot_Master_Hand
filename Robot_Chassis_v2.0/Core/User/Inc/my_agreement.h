/**
 * @file    my_agreement.h
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
#ifndef MY_AGREEMENT_H
#define MY_AGREEMENT_H

#include "main.h"

typedef struct
{
   uint8_t head[2];
   uint8_t len1;       /* 长度 */
   uint8_t len2;
   uint8_t driver;     /* 设备号 */
   uint8_t kind;       /* 类型 */
   uint8_t version;    /* 版本号 */
   uint8_t answer;     /* 是否应答 */
   uint8_t index[2];
   uint8_t ID;
   uint8_t crc[2];
   uint8_t tail[2];
   struct _ringbuffer_t *pack_data_index;
} General_Frame;

extern General_Frame LeftArm_Frame;
extern General_Frame RightArm_Frame;
extern General_Frame Boot_Frame;

void GeneralFrameUPInit(General_Frame *_this, uint8_t _driver, uint8_t *_KindId, struct _ringbuffer_t *_buffer);
void AgreementEncapsula(General_Frame *_this, uint8_t *_trans_data);
void DataEncapsula(General_Frame *_this, const int16_t *_angle);
uint16_t GetPackSize(General_Frame *_this);
uint16_t Crc16Verify(uint8_t *ptr, uint16_t len);
void BootPack(General_Frame *_this,uint8_t * _data_32,uint8_t _canid);
#endif //MY_AGREEMENT_H
