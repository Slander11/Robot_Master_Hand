
/**
 * @file    my_key.c
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

#include "../Inc/my_key.h"

uint8_t g_Key_Matrix[ROWS][COLS] = {0};        /* 按键状态数组 */
uint8_t g_Key_Matrix2[4] = {0};                /* 按键状态数组2 */

GPIO_TypeDef* row_ports[ROWS] = {GPIOA, GPIOA, GPIOA};
uint16_t row_pins[ROWS] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2};

GPIO_TypeDef* col_ports[COLS] = {GPIOA, GPIOA, GPIOA};
uint16_t col_pins[COLS] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5};

GPIO_TypeDef* key_ports[4] = {GPIOB, GPIOA, GPIOA , GPIOA};
uint16_t key_pins[4] = {Enable_Switch_Pin, Toggle_Switch_Up_Pin, Toggle_Switch_Mid_Pin, Toggle_Switch_Down_Pin};


/**
  * @brief       3*3 按键扫描
  * @param       **
  * @retval      **
  */
void KeyScan(void)
{
    static uint8_t last_key_state[ROWS][COLS] = {0};
    static uint32_t debounce_time[ROWS][COLS] = {0};

    uint8_t current_state;

    for(int row=0; row<ROWS; row++) {
        /* 拉低当前行 */
        HAL_GPIO_WritePin(row_ports[row], row_pins[row], GPIO_PIN_RESET);

        /* 短暂延时确保电平稳定 */
        HAL_Delay(1);

        /* 读取所有列状态 */
        for(int col=0; col<COLS; col++) {
            current_state = (HAL_GPIO_ReadPin(col_ports[col], col_pins[col]) == GPIO_PIN_RESET) ? 1 : 0;

            /* 消抖处理 */
            if(current_state != last_key_state[row][col]) {
                debounce_time[row][col] = HAL_GetTick();
                last_key_state[row][col] = current_state;
            }

            /* 状态稳定超过10ms则更新矩阵 */
            if((HAL_GetTick() - debounce_time[row][col]) > 10) {
                g_Key_Matrix[row][col] = current_state;
            }
        }

        /* 恢复当前行为高电平 */
        HAL_GPIO_WritePin(row_ports[row], row_pins[row], GPIO_PIN_SET);
    }

    for(int i = 0; i < 4; i++) {
      g_Key_Matrix2[i] = HAL_GPIO_ReadPin(key_ports[i], key_pins[i]);
    }
}