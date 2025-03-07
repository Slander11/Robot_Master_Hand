
/**
 * @file    my_ringbuff.c
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

#include "../Inc/my_ringbuff.h"

uint8_t Usb_Buff[BUFF_SIZE] = {};
uint8_t LeftArm_Buff[BUFF_SIZE] = {};
uint8_t RightArm_Buff[BUFF_SIZE] = {};
uint8_t Boot_Buff[BUFF_SIZE] = {};

RingBuff_t BootLoader_RingBuff_t;
RingBuff_t Usb_RingBuff_t;
RingBuff_t LeftArm_RingBuff_t;
RingBuff_t RightArm_RingBuff_t;
/**
 * @brief       初始化环形数据
 * @param       pRingBuff  : 新的环形缓冲区
                pBuff      : 初始化数组
                buf_size   : 大小
 * @retval      **
 */
void RingBuffInit(RingBuff_t *_pRingBuff, uint8_t *_pBuff, uint16_t _buf_size)
{
    if (NULL == _pRingBuff || NULL == _pBuff)
        return;
    _pRingBuff->head = 0;
    _pRingBuff->tail = 0;
    _pRingBuff->size = _buf_size;
    _pRingBuff->buffer = _pBuff;
}

/**
 * @brief       判空
 * @param       _pRingBuff  : 环形结构体
 * @retval      RING_FALSE : 未空   RING_TRUE : 空
 */
int RingBuffEmpty(RingBuff_t *_pRingBuff)
{
    if (NULL == _pRingBuff)
        return RING_FALSE;

    if (_pRingBuff->head == _pRingBuff->tail)
    {
        return RING_TRUE;
    }
    return RING_FALSE;
}

/**
 * @brief       判满
 * @param       _pRingBuff  : 环形结构体
 * @retval      RING_FALSE : 未满 RING_TRUE : 满
 */
static int RingBuffFull(RingBuff_t *_pRingBuff)
{
    if (NULL == _pRingBuff)
        return RING_FALSE;

    if ((_pRingBuff->tail + 1) % _pRingBuff->size == _pRingBuff->head)
    {
        return RING_TRUE;
    }
    return RING_FALSE;
}

/**
 * @brief       从环形缓冲区读取一个字节
 * @param       _pRingBuff  : 环形缓冲区
 * @retval      RING_ERR : 出错 RING_OK : 正常
 */
static int RingBuffGetOneByte(RingBuff_t *_pRingBuff, uint8_t *_pData)
{
    if (NULL == _pRingBuff || NULL == _pData)
        return RING_ERR;

    /* 判空 */
    if (RING_TRUE == RingBuffEmpty(_pRingBuff))
    {
        return RING_ERR;
    }

    *_pData = _pRingBuff->buffer[_pRingBuff->head];
    _pRingBuff->head = (_pRingBuff->head + 1) % _pRingBuff->size;
    return RING_OK;
}

/**
 * @brief       从环形缓冲区读取N个字节
 * @param       _pRingBuff  : 环形缓冲区
                _pData      : 用于存储读取数据得数组
                _size       : 读取的字节数
 * @retval      RING_ERR : 出错 RING_OK : 正常
 */
int RingBuffReadNByte(RingBuff_t *_pRingBuff, uint8_t *_pData, uint16_t _size)
{
    if (NULL == _pRingBuff || NULL == _pData)
        return RING_ERR;

    // 判空
    if (RING_TRUE == RingBuffEmpty(_pRingBuff))
    {
        return RING_ERR;
    }
    uint16_t now_index = _pRingBuff->head;
    for (size_t i = 0; i < _size; i++)
    {
        _pData[i] = _pRingBuff->buffer[now_index];
        now_index = (_pRingBuff->head + 1 + i) % _pRingBuff->size;
    }
    return RING_OK;
}

/**
 * @brief       向环形缓冲区写入一个字节
 * @param       _pRingBuff  : 环形缓冲区
                _pData      : 写入得数据
 * @retval      RING_ERR : 写入出错 RING_OK : 正常写入
 */
static int RingBuffWriteOneByte(RingBuff_t *_pRingBuff, uint8_t *_pData)
{
    if (NULL == _pRingBuff || NULL == _pData)
        return RING_ERR;

    // 判满
    if (RING_TRUE == RingBuffFull(_pRingBuff))
    {
        return RING_ERR;
    }
    _pRingBuff->buffer[_pRingBuff->tail] = *_pData;
    _pRingBuff->tail = (_pRingBuff->tail + 1) % _pRingBuff->size;
    return RING_OK;
}

/**
 * @brief       获得环形缓冲区N个字节
 * @param       _pRingBuff  : 环形缓冲区
                _pData      : 获得数据得数组
                _size       : 获取得字节数
 * @retval      RING_ERR : 写入出错 RING_OK : 正常写入
 */
int RingBuffGetNByte(RingBuff_t *_pRingBuff, uint8_t *_pData, uint16_t _size)
{
    if (NULL == _pRingBuff || NULL == _pData)
        return RING_ERR;

    for (int i = 0; i < _size; i++)
    {
        RingBuffGetOneByte(_pRingBuff, _pData + i);
    }
    return RING_OK;
}

/**
 * @brief       向环形缓冲区写入N个字节
 * @param       _pRingBuff  : 环形缓冲区
                _pData      : 写入得数据
                _size       : 写入的字节数
 * @retval      RING_ERR : 出错 RING_OK : 正常
 */
int RingBuffWriteNByte(RingBuff_t *_pRingBuff, uint8_t *_pData, uint16_t _size)
{
    if (NULL == _pRingBuff || NULL == _pData)
        return RING_ERR;

    for (int i = 0; i < _size; i++)
    {
        RingBuffWriteOneByte(_pRingBuff, _pData + i);
    }
    return RING_OK;
}

/**
 * @brief       获取当前环形缓冲区长度
 * @param       _pRingBuff  : 环形缓冲区
 * @retval      长度
 */
uint16_t RingBuffGetLen(RingBuff_t *_pRingBuff)
{
    if (NULL == _pRingBuff)
        return RING_ERR;

    if (_pRingBuff->tail >= _pRingBuff->head)
    {
        return _pRingBuff->tail - _pRingBuff->head;
    }

    return _pRingBuff->tail + _pRingBuff->size - _pRingBuff->head;
}

/**
 * @brief       移除当前环形缓冲区1个数据
 * @param       _pRingBuff  : 环形缓冲区
 * @retval      RING_OK     : 成功  RING_ERR     : 出错
 */
int RingBuffRemoveOneByte(RingBuff_t *_pRingBuff)
{
    /* 判空 */
    if (RING_TRUE == RingBuffFull(_pRingBuff))
    {
        return RING_ERR;
    }
    _pRingBuff->head = (_pRingBuff->head + 1) % _pRingBuff->size;
    return RING_OK;
}

/**
 * @brief       移除当前环形缓冲区N个数据
 * @param       _pRingBuff  : 环形缓冲区
                _size       : 移除得数据长度
 * @retval      RING_OK     : 成功
 */
int RingBuffRemoveNByte(RingBuff_t *_pRingBuff, uint16_t _size)
{
    for (int i = 0; i < _size; i++)
    {
        if (RING_ERR == RingBuffRemoveOneByte(_pRingBuff)) {
            return RING_ERR;
        }
    }
    return RING_OK;
}

/**
 * @brief       清空环形缓冲区数据
 * @param       _pRingBuff  : 环形缓冲区
 * @retval      **
 */
void RingBuffClear(RingBuff_t *_pRingBuff)
{
    _pRingBuff->head = _pRingBuff->tail;
}

/**
 * @brief       获取环形缓冲区头数据
 * @param       _pRingBuff  : 环形缓冲区
 * @retval      失败 ：RING_ERR 成功 ： 返回头部数据
 */
uint8_t RingBuffGetHeadItem(RingBuff_t *_pRingBuff)
{
    if (NULL == _pRingBuff) {
        return RING_ERR;
    }
    return _pRingBuff->buffer[_pRingBuff->head];
}

/**
 * @brief       获取环形缓冲区指定下标数据
 * @param       _pRingBuff  : 环形缓冲区
                _index      : 下标
 * @retval      失败 ：RING_ERR 成功 ： 返回指定下标数据
 */
uint8_t RingBuffGetIndexItem(RingBuff_t *_pRingBuff, uint16_t _index)
{
    if (NULL == _pRingBuff || _index > _pRingBuff->size - 1) {
        return RING_ERR;
    }
    uint16_t new_index = (_pRingBuff->head + _index) % _pRingBuff->size;
    return _pRingBuff->buffer[new_index];
}

/**
 * @brief       向环形缓冲区指定下标写入数据
 * @param       _pRingBuff  : 环形缓冲区
                _index      : 下标
                _data       : 要写入得数据
 * @retval      失败 ：RING_ERR 成功 ： RING_OK
 */
uint8_t RingBuffReWriteIndexItem(RingBuff_t *_pRingBuff, uint16_t _index, uint8_t _data)
{
    if (NULL == _pRingBuff || _index > _pRingBuff->size - 1) {
        return RING_ERR;
    }
    uint16_t new_index = (_pRingBuff->head + _index) % _pRingBuff->size;
    _pRingBuff->buffer[new_index] = _data;
    return RING_OK;
}


