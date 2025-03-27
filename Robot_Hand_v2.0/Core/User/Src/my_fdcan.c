
/**
 * @file    my_fdcan.c
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

#include "../Inc/my_fdcan.h"

extern FDCAN_HandleTypeDef hfdcan1;

FDCAN_TxHeaderTypeDef TXHeader;
FDCAN_RxHeaderTypeDef RXHeader;

extern uint32_t g_ChipId;
extern uint8_t g_SendData[16];
extern uint16_t g_Adc_Buffer[2];
static void CanFilterInit(void);
static uint8_t GetFDcanData(FDCAN_RxHeaderTypeDef *_RxHeader,uint8_t *RxMsg);
static void ReceiveAnalyse(uint8_t *_data, uint32_t _size);
/**
 * @brief       canfd初始化
 * @param       **
 * @retval      **
 */
void CanInit(void)
{
    /* 设置滤波器 */
    CanFilterInit();
    /* 开启接收中断 */
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    HAL_FDCAN_Start(&hfdcan1);
}

/**
 * @brief       canfd滤波设置
 * @param       **
 * @retval      **
 */
static void CanFilterInit(void)
{
    HAL_StatusTypeDef HAL_Status;
    FDCAN_FilterTypeDef sFilterConfig;

    /**配置CAN过滤器*/
    sFilterConfig.IdType = FDCAN_STANDARD_ID;             /* 标准ID */
    sFilterConfig.FilterIndex = 0;                        /* 滤波器索引 */
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; /* 过滤器0关联到FIFO0 */
    sFilterConfig.FilterType = FDCAN_FILTER_DUAL;         /* 掩码过滤 */
    sFilterConfig.FilterID1 = 0x100;
    sFilterConfig.FilterID2 = 0x200 | flash_param.can_id;

    HAL_Status = HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig);
    if (HAL_Status != HAL_OK)
    {
        Error_Handler();
    }
}

/**
 * @brief       CANFD 发送一组数据
 * @param       id      : 标准ID(11位)
 *              msg     : 数据
 *              len     : 长度
 * @retval      发送状态 0, 成功; 其余, 失败;
 */
uint8_t CanSendMsg(uint32_t _id, uint8_t *_msg, uint32_t _len)
{
    HAL_StatusTypeDef state;

    TXHeader.Identifier = _id;
    TXHeader.IdType = FDCAN_STANDARD_ID;
    TXHeader.TxFrameType = FDCAN_DATA_FRAME;
    TXHeader.DataLength = _len;
    TXHeader.ErrorStateIndicator = FDCAN_ESI_PASSIVE;
    TXHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TXHeader.FDFormat = FDCAN_FD_CAN;
    TXHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TXHeader.MessageMarker = 0x00;

    if ((hfdcan1.Instance->TXFQS & FDCAN_TXFQS_TFQF) != 0U)
    {
        return 0;
    }
    state = HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TXHeader, _msg);
    if (state != HAL_OK)
        return state; /* 发送状态 */
    return 0;
}

/**
 * @brief       canfd发送手柄数据函数
 * @param       **
 * @retval      **
 */
void SendHandState()
{
    if (0 == flash_param.can_id) {
        /* 发送id */
        uint32_t id = 0x100;
        BitToUint32_value trans;
        trans.uint32 = g_ChipId;
        CanSendMsg(id, trans.bit, FDCAN_DLC_BYTES_4);
    }
    else {
        /* 发送角度 */
        uint32_t id = 0x100 | flash_param.can_id;

        CanSendMsg(id, g_SendData, FDCAN_DLC_BYTES_20);
    }
}

/**
 * @brief       canfd中断接收回调函数
 * @param       hfdcan ： canfd句柄
 * @retval      **
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *_hfdcan, uint32_t _RxFifo0ITs)
{
    uint8_t RxMsg[64];
    if (_hfdcan == &hfdcan1)
    {
        // 新消息处理
        if ((_RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
        {
            /* 从RX FIFO0读取数据 */
            HAL_FDCAN_GetRxMessage(_hfdcan, FDCAN_RX_FIFO0, &RXHeader, RxMsg);

            /* 获取数据 */
            GetFDcanData(&RXHeader,RxMsg);

            /* 重新使能RX FIFO0阈值中断 */
            HAL_FDCAN_ActivateNotification(_hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
        }
            // fifo满处理
        else if ((_RxFifo0ITs & FDCAN_IT_RX_FIFO0_FULL) != RESET)
        {
            for (size_t i = 0; i < 2; i++)
            {
                HAL_FDCAN_GetRxMessage(_hfdcan, FDCAN_RX_FIFO0, &RXHeader, RxMsg);
            }
            HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_FULL, 0);
        }
    }
}

/**
 * @brief       接收数据处理函数
 * @param       RxHeader  ：FDCAN Rx header structure definition
 * @retval      TXmessage ：Data
 */
static uint8_t GetFDcanData(FDCAN_RxHeaderTypeDef *_RxHeader,uint8_t *_TXmessage)
{
    if (0x100 == _RxHeader->Identifier)
    {
        /* 修改canid */
        BitToUint32_value trans;
        for (size_t i = 0; i < 4; i++)
        {
            trans.bit[i] = _TXmessage[i];
        }
        if (trans.uint32 == g_ChipId)
        {
            flash_param.can_id = _TXmessage[4];
            /* 重写falsh */
            WriteFlashDefault(1);
        }
    }
    else if ((0x200 | flash_param.can_id) == _RxHeader->Identifier)
    {
        ReceiveAnalyse(_TXmessage, _RxHeader->DataLength);
    }
}

/**
 * @brief       修改方向
 * @param       **
 * @retval      **
 */
static void ReviseDirection(void)
{
    if(flash_param.direction == 0)
    {
        flash_param.direction = 1;
    }
    else
    {
        flash_param.direction = 0;
    }
}

static void ReceiveAnalyse(uint8_t *_data, uint32_t _size)
{
    if (_size == 1)
    {
        switch (_data[0])
        {
            case 0x01:
                /* 重新保存数据 */
                    WriteFlashDefault(1);
            break;
            case 0x10:
                /* 编码器置零 */
                    flash_param.zero = ((uint32_t)g_Adc_Buffer[0] * 0x1FFF / 2650);
            break;
            case 0x11:
                /* 修改编码器方向 */
                    ReviseDirection();
            break;
            case 0xbb:
                /* 下次启动升级 */
                    WriteFlashDefault(2);
            break;
            case 0xff:
                /* 软重启 */
                    NVIC_SystemReset();
            break;
            default:
                break;
        }
    }
}
