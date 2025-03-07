
/**
 * @file    my_ams.c
 * @brief   简要描述本文件功能
 * @author  yanyan6.liu
 * @date    2025/3/6
 * @version 1.0.0
 * @license 许可证
 * 
 * 修改记录：
 * | version |    data    |    author   |       beizhu      |
 * |---------|------------|-------------|-------------------|
 * |  1.0.0  | 2025-03-06 | yanyan6.liu |       初始版本      |
 */

#include "../Inc/my_ams.h"

extern SPI_HandleTypeDef hspi1;

uint16_t g_AmsError;
uint16_t g_AmsOriginAngle;
uint16_t g_AmsDataprocessAngle;

/**
 * @brief       读取编码器参数
 * @param       _sensor : 用于保存的参数
 * @retval      **
 */
void ReadAmsSensor(uint16_t *_sensor)
{
    SPI1_CS_RESET;
    uint16_t data = TransReceive16(READ_ANGLECOM);
    SPI1_CS_SET;
    // 检查错误及校验
    if (data & (1 << 14)) {
        // 有错误 发送清除错误指令
        TransReceive16(READ_ERRFL);
        // 获取错误原因
        g_AmsError = TransReceive16(READ_NOP);
        g_AmsError = g_AmsError & 0x0003;
        return;
    } else {
        // 奇偶校验
        // 偶校验最高位 通过则读取数据
        if ((data >> 15) == EvenCheck(data & 0x7FFF)) {
            *_sensor = data & 0x3fff;
        } else {
            return;
        }
    }
    //    *sensor = data & 0x3fff;
}

/**
 * @brief       读取编码器转换高低字节
 * @param       _trans : 命令
 * @retval      读取的值
 */
uint16_t TransReceive16(uint16_t _trans)
{
    uint8_t trans_data[2], receive_data[2];
    trans_data[0] = _trans >> 8;
    trans_data[1] = _trans & 0xff;
    HAL_SPI_TransmitReceive(&hspi1, trans_data, receive_data, 2, 2);
    uint16_t res;
    res = receive_data[0];
    res = (res << 8) + receive_data[1];
    return res;
}

/**
 * @brief       奇偶校验
 * @param       _v : 需要校验得值
 * @retval      **
 */
unsigned int EvenCheck(unsigned int _v)
{
    if (_v == 0) return 0;
    _v ^= _v >> 8;
    _v ^= _v >> 4;
    _v ^= _v >> 2;
    _v ^= _v >> 1;
    return _v & 1;
}

/**
 * @brief       编码器数据处理
 * @param       **
 * @retval      处理后的数据
 */
uint16_t SendDataProcess(void)
{
    /* 编码器数据处理 */
    /* 填充至16位 */
    uint16_t temp_angle = g_AmsOriginAngle << 3;
    /* 处理零点 */
    temp_angle = temp_angle - (flash_param.zero << 3);
    /* 处理方向 */
    if (flash_param.direction == 1)
    {
       temp_angle = 65535 - temp_angle;
    }
    /* 转换为int16 */
    int16_t temp_angle2 = (int16_t)temp_angle;
    temp_angle2 = (int32_t)temp_angle2 * 360 * 128 / 65535;
    return (int16_t)temp_angle2;
}