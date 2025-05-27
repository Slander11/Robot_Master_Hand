
/**
 * @file    my_agreement.c
 * @brief   简要描述本文件功能
 * @author  yanyan6.liu
 * @date    2025/3/5
 * @version 1.0.0
 * @license 许可证
 * 
 * 修改记录：
 * | version |    data    |    author   |       beizhu      |
 * |---------|------------|-------------|-------------------|
 * |  1.0.0  | 2025-03-05 | yanyan6.liu |       初始版本      |
 */

#include "../Inc/my_agreement.h"

General_Frame LeftArm_Frame;
General_Frame RightArm_Frame;
General_Frame Boot_Frame;

uint16_t s_updata_index = 0xfff0;

/**
 * @brief       协议初始化
 * @param       _this   ：协议通用结构体
                _driver ： 设备号
                _ID     ： ID号
                _buffer :  环形缓冲区
 * @retval      **
 */
void GeneralFrameUPInit(General_Frame *_this, uint8_t _driver, uint8_t *_KindId, struct _ringbuffer_t *_buffer)
{
    _this->pack_data_index = _buffer;
    /* 帧头 */
    _this->head[0] = 0x02;
    _this->head[1] = 0xfd;
    /* 长度 */
    BitToHex_value trans_16;
    trans_16.hex = 15;
    _this->len2 = trans_16.bit[0];
    _this->len1 = trans_16.bit[1];
    /* 设备号 */
    _this->driver = _driver;
    /* 类型 */
    _this->kind = _KindId[0];
    /* 版本 */
    _this->version = 0x01;
    /* 索引 */
    trans_16.hex = s_updata_index;
    _this->index[0] = trans_16.bit[1];
    _this->index[1] = trans_16.bit[0];
    _this->ID = _KindId[1];
    /* 帧尾 */
    _this->tail[0] = 0x03;
    _this->tail[1] = 0xfc;
    /* 数据归零 */
    RingBuffClear(_this->pack_data_index);
    /* 打包 数据集 */
    RingBuffWriteNByte(_this->pack_data_index, _this->head, 2);
    RingBuffWriteNByte(_this->pack_data_index, &_this->len1, 1);
    RingBuffWriteNByte(_this->pack_data_index, &_this->len2, 1);
    RingBuffWriteNByte(_this->pack_data_index, &_this->driver, 1);
    RingBuffWriteNByte(_this->pack_data_index, &_this->kind, 1);
    RingBuffWriteNByte(_this->pack_data_index, &_this->version, 1);
    RingBuffWriteNByte(_this->pack_data_index, &_this->answer, 1);
    RingBuffWriteNByte(_this->pack_data_index, _this->index, 2);
    RingBuffWriteNByte(_this->pack_data_index, &_this->ID, 1);
    /* 索引递增 */
    s_updata_index++;
}

/**
 * @brief       协议封装
 * @param       _this   ：协议通用结构体
                _trans_data ： 协议封装后存储数组
 * @retval      **
 */
void AgreementEncapsula(General_Frame *_this, uint8_t *_trans_data)
{
    union BitToHex data_trans_16;
    // 计算总长，更改长度字段
    data_trans_16.hex = RingBuffGetLen(_this->pack_data_index);
    data_trans_16.hex = data_trans_16.hex + 4; // 加上CRC与帧尾长度
    RingBuffReWriteIndexItem(_this->pack_data_index, 2, data_trans_16.bit[1]);
    RingBuffReWriteIndexItem(_this->pack_data_index, 3, data_trans_16.bit[0]);
    // 计算crc
    uint8_t crc_buff[128];
    uint16_t data_size = RingBuffGetLen(_this->pack_data_index);
    RingBuffReadNByte(_this->pack_data_index, crc_buff, data_size);
    data_trans_16.hex = Crc16Verify(crc_buff, data_size);
    RingBuffWriteNByte(_this->pack_data_index, data_trans_16.bit, 2);
    // 添加帧尾
    RingBuffWriteNByte(_this->pack_data_index, _this->tail, 2);
    // 添加至缓存
    RingBuffReadNByte(_this->pack_data_index, _trans_data, RingBuffGetLen(_this->pack_data_index));
}

/**
 * @brief       数据封装
 * @param       _this   ：协议通用结构体
                _trans_data ： 协议封装后存储数组
 * @retval      **
 */
void DataEncapsula(General_Frame *_this, const int16_t *_angle)
{
    /* 编码器数据 */
    for (int i = 0; i < 8; ++i)
    {
        BitToHex_value data_trans_16;
        data_trans_16.hex = _angle[i];
        RingBuffWriteNByte(_this->pack_data_index, &data_trans_16.bit[1], 1);
        RingBuffWriteNByte(_this->pack_data_index, &data_trans_16.bit[0], 1);
    }

    /* 按键数据 */
    if(_this->driver == 6)
    {
        for (int i = 0; i < 17; i++)
        {
            if (i == 13) {
                RingBuffWriteNByte(_this->pack_data_index, &g_key_temp[0], 1);
            }
            else {
                RingBuffWriteNByte(_this->pack_data_index, &g_key_Buff[i], 1);
            }
        }
    }
    else
    {
        for (int i = 17; i < 34; i++)
        {
            if (i == 30) {
                RingBuffWriteNByte(_this->pack_data_index, &g_key_temp[1], 1);
            }
            else {
                RingBuffWriteNByte(_this->pack_data_index, &g_key_Buff[i], 1);
            }

        }
    }
    RingBuffWriteNByte(_this->pack_data_index,&foot_data,1);
    /* 占空 */
    uint8_t Duy[2] = {};
    RingBuffWriteNByte(_this->pack_data_index,Duy,2);
}

/**
 * @brief       获取协议包大小
 * @param       _this   ：协议通用结构体
 * @retval      大小
 */
uint16_t GetPackSize(General_Frame *_this)
{
    return RingBuffGetLen(_this->pack_data_index);
}

/**
 * @brief       boot id封装
 * @param       _this   ：协议通用结构体
                _data_32 : 数据包
                _canid   : 指定id号
 * @retval      大小
 */
void BootPack(General_Frame *_this,uint8_t * _data_32,uint8_t _canid)
{
    RingBuffWriteNByte(_this->pack_data_index, _data_32, 32);
    RingBuffWriteNByte(_this->pack_data_index, &_canid, 1);
}
/**
 * @brief       CRC16校验
 * @param       _ptr   ：校验得数据
                _len   : 长度
 * @retval      CRC校验后得值
 */
uint16_t Crc16Verify(uint8_t *_ptr, uint16_t _len)
{
    uint16_t wcrc = 0xFFFF;         /* 预置16位crc寄存器，初值全部为1 */
    uint16_t temp;                  /* 定义中间变量 */
    int i, j;                       /* 定义计数 */
    for (i = 0; i < _len; i++)      /* 循环计算每个数据 */
    {
        temp = *_ptr & 0X00FF;      /* 将八位数据与crc寄存器亦或 */
        _ptr++;                     /* 指针地址增加，指向下个数据 */
        wcrc ^= temp;               /* 将数据存入crc寄存器 */
        for (j = 0; j < 8; j++)     /* 循环计算数据的 */
        {
            if (wcrc & 0X0001)      /* 判断右移出的是不是1，如果是1则与多项式进行异或 */
            {
                wcrc >>= 1;         /* 先将数据右移一位 */
                wcrc ^= 0XA001;     /* 与上面的多项式进行异或 */
            }
            else
            {
                wcrc >>= 1;         /* 直接移出 */
            }
        }
    }
    temp = wcrc;                    /* crc的值 */
    return temp;
}

