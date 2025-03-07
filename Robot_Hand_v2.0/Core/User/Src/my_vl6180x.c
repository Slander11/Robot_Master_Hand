
/**
 * @file    my_vl6180x.c
 * @brief   简要描述本文件功能
 * @author  yanyan6.liu
 * @date    2025/3/4
 * @version 1.0.0
 * @license 许可证
 * 
 * 修改记录：
 * | version |    data    |    author   |       beizhu      |
 * |---------|------------|-------------|-------------------|
 * |  1.0.0  | 2025-03-04 | yanyan6.liu |       初始版本      |
 */

#include "../Inc/my_vl6180x.h"

extern I2C_HandleTypeDef hi2c1;

/**
 * @brief       写8位
 * @param		**
 * @retval      **
 */
static void VL6180X_WriteByte(uint16_t _reg,uint8_t _data)

{
	HAL_I2C_Mem_Write(&hi2c1,(VL6180X_DEFAULT_I2C_ADDR<<1)|0,_reg,I2C_MEMADD_SIZE_16BIT,&_data,1,0xff);
}

/**
 * @brief       读8位
 * @param		**
 * @retval      读取得值
 */
static uint8_t VL6180X_ReadByte(uint16_t _reg)

{
    uint8_t data=0;
    HAL_I2C_Mem_Read(&hi2c1 ,(VL6180X_DEFAULT_I2C_ADDR << 1)|1,_reg,I2C_MEMADD_SIZE_16BIT,&data,1,0xffff);
    return data;
}

/**
 * @brief       读ID
 * @param		**
 * @retval      **
 */
uint8_t VL6180X_Read_ID()
{
	return VL6180X_ReadByte(VL6180X_REG_IDENTIFICATION_MODEL_ID);
}

/**
 * @brief       传感器初始化
 * @param		**
 * @retval      0：成功  1：失败
 */
uint8_t VL6180X_Init()
{
	uint8_t ptp_offset = 0;
	ptp_offset = VL6180X_ReadByte(0x16);
	if(0x01 == ptp_offset)
	{
		VL6180X_WriteByte(0x0207, 0x01);
		VL6180X_WriteByte(0x0208, 0x01);
		VL6180X_WriteByte(0x0096, 0x00);
		VL6180X_WriteByte(0x0097, 0xfd);
		VL6180X_WriteByte(0x00e3, 0x00);
		VL6180X_WriteByte(0x00e4, 0x04);
		VL6180X_WriteByte(0x00e5, 0x02);
		VL6180X_WriteByte(0x00e6, 0x01);
		VL6180X_WriteByte(0x00e7, 0x03);
		VL6180X_WriteByte(0x00f5, 0x02);
		VL6180X_WriteByte(0x00d9, 0x05);
		VL6180X_WriteByte(0x00db, 0xce);
		VL6180X_WriteByte(0x00dc, 0x03);
		VL6180X_WriteByte(0x00dd, 0xf8);
		VL6180X_WriteByte(0x009f, 0x00);
		VL6180X_WriteByte(0x00a3, 0x3c);
		VL6180X_WriteByte(0x00b7, 0x00);
		VL6180X_WriteByte(0x00bb, 0x3c);
		VL6180X_WriteByte(0x00b2, 0x09);
		VL6180X_WriteByte(0x00ca, 0x09);
		VL6180X_WriteByte(0x0198, 0x01);
		VL6180X_WriteByte(0x01b0, 0x17);
		VL6180X_WriteByte(0x01ad, 0x00);
		VL6180X_WriteByte(0x00ff, 0x05);
		VL6180X_WriteByte(0x0100, 0x05);
		VL6180X_WriteByte(0x0199, 0x05);
		VL6180X_WriteByte(0x01a6, 0x1b);
		VL6180X_WriteByte(0x01ac, 0x3e);
		VL6180X_WriteByte(0x01a7, 0x1f);
		VL6180X_WriteByte(0x0030, 0x00);

		// Recommended : Public registers - See data sheet for more detail
		VL6180X_WriteByte(0x0011, 0x10);       // Enables polling for 'New Sample ready'
									// when measurement completes
		VL6180X_WriteByte(0x010a, 0x30);       // Set the averaging sample period
									// (compromise between lower noise and
									// increased execution time)
		VL6180X_WriteByte(0x003f, 0x46);       // Sets the light and dark gain (upper
									// nibble). Dark gain should not be
									// changed. !上半字节要写入0x4	默认增益是1.0
		VL6180X_WriteByte(0x0031, 0xFF);       // sets the # of range measurements after
									// which auto calibration of system is
									// performed
		VL6180X_WriteByte(0x0041, 0x63);       // Set ALS integration time to 100ms
		VL6180X_WriteByte(0x002e, 0x01);       // perform a single temperature calibration
									// of the ranging sensor

		// Optional: Public registers - See data sheet for more detail
		VL6180X_WriteByte(0x001b, 0x09);    //测量间隔	轮询模式
									// period to 100ms	每步10ms->0-10ms
		VL6180X_WriteByte(0x003e, 0x31);      //测量周期	ALS模式
									// to 500ms
		VL6180X_WriteByte(0x0014, 0x24);       // Configures interrupt on 'New Sample
									// Ready threshold event'

		VL6180X_WriteByte(0x24, 0x7f);
		VL6180X_WriteByte(0x0016, 0x00);
		return 0;
	  }
      else return 1;
  }

/**
 * @brief       读数据
 * @param		_range 数据保存
 * @retval      **
*/
void VL6180X_Read_Range(uint8_t *_range)
{
	//开启传输
	while(!(VL6180X_ReadByte(VL6180X_REG_RESULT_RANGE_STATUS) & 0x01));
	VL6180X_WriteByte(VL6180X_REG_SYSRANGE_START,0x01);	//单次触发模式
	//等待新样本就绪阈值事件(New Sample Ready threshold event)
	while(!(VL6180X_ReadByte(VL6180X_REG_RESULT_INTERRUPT_STATUS_GPIO) & 0x04));
	*_range = VL6180X_ReadByte(VL6180X_REG_RESULT_RANGE_VAL);
	//获取完数据，清楚中断位
	VL6180X_WriteByte(VL6180X_REG_SYSTEM_INTERRUPT_CLEAR,0x07);	//0111b 清除了三种中断标志
}

