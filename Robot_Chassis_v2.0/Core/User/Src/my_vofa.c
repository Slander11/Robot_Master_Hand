
/**
 * @file    my_vofa.c
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

#include "../Inc/my_vofa.h"

Frame_t BoaruUploadBuff;
void BOARDUploadStart(void) {
    CDC_Transmit_FS((uint8_t *)&BoaruUploadBuff, sizeof(Frame_t));
}
