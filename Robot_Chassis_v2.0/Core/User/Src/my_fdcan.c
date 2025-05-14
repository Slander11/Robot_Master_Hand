
/**
 * @file    my_fdcan.c
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

#include "../Inc/my_fdcan.h"


extern FDCAN_HandleTypeDef hfdcan1;
extern uint32_t g_ChipId;

FDCAN_TxHeaderTypeDef TXHeader;
FDCAN_RxHeaderTypeDef RXHeader;

int16_t g_lAngel_Buff[8] = {};                /* 左关节角度 */
int16_t g_rAngel_Buff[8] = {};                /* 右关节角度 */
uint8_t g_key_Buff[34] = {};                  /* 按键状态 */

static void CanFilterInit(void);
static uint8_t GetFDcanData(FDCAN_RxHeaderTypeDef *_RxHeader,uint8_t *_RxMsg);
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
    sFilterConfig.FilterType = FDCAN_FILTER_RANGE;         /* 掩码过滤 */
    sFilterConfig.FilterID1 = 0x0000;
    sFilterConfig.FilterID2 = 0xffff;

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
static void AnalyseData(FDCAN_RxHeaderTypeDef *_RxHeader,uint8_t *_RxMsg,int16_t *_lAngel, int16_t *_rAngel, uint8_t *_Key)
{
    BitToHex_value trans_data;
    uint32_t cur_signal = 0;
    static uint32_t ber_signal = 0;
    for (uint8_t i = 0; i < 7; i++) {
        if (_RxHeader->Identifier == 0x101 + i && _RxHeader->DataLength == FDCAN_DLC_BYTES_2) {
            trans_data.bit[0]=_RxMsg[0];
            trans_data.bit[1]=_RxMsg[1];
            g_lAngel_Buff[i]=trans_data.hex;
        }
        if (_RxHeader->Identifier == 0x111 + i && _RxHeader->DataLength == FDCAN_DLC_BYTES_2) {
            trans_data.bit[0]=_RxMsg[0];
            trans_data.bit[1]=_RxMsg[1];
            g_rAngel_Buff[i]=trans_data.hex;
        }
    }
    if (_RxHeader->Identifier == 0x108 && _RxHeader->DataLength == FDCAN_DLC_BYTES_20) {
        for (uint8_t i = 0; i < 14; i++) {
            g_key_Buff[i] = _RxMsg[i];
        }
        trans_data.bit[0] = _RxMsg[14];
        trans_data.bit[1] = _RxMsg[15];
        g_lAngel_Buff[7] = trans_data.hex;
    }
    else if (_RxHeader->Identifier == 0x118 && _RxHeader->DataLength == FDCAN_DLC_BYTES_20) {
        for (uint8_t i = 0; i < 14; i++) {
            g_key_Buff[i + 16] = _RxMsg[i];
        }
        trans_data.bit[0] = _RxMsg[14];
        trans_data.bit[1] = _RxMsg[15];
        g_rAngel_Buff[7] = trans_data.hex;
    }

    /* 172 - 197 临时修改得程序，兼容上一代 */
    // if (g_key_Buff[13] <= 85) {
    //     g_key_Buff[1] = 0;
    //     g_key_Buff[0] = 1;
    // }else if (g_key_Buff[13] >= 92){
    //     g_key_Buff[1] = 1;
    //     g_key_Buff[0] = 0;
    // }else {
    //     g_key_Buff[1] = 0;
    //     g_key_Buff[0] = 0;
    // }
    // g_key_Buff[2] = g_key_Buff[4];
    // g_key_Buff[3] = g_key_Buff[9];
    //
    // if (g_key_Buff[27] <= 85) {
    //     g_key_Buff[15] = 0;
    //     g_key_Buff[14] = 1;
    // }else if (g_key_Buff[27] >= 92){
    //     g_key_Buff[15] = 1;
    //     g_key_Buff[14] = 0;
    // }else {
    //     g_key_Buff[15] = 0;
    //     g_key_Buff[14] = 0;
    // }
    // g_key_Buff[16] = g_key_Buff[18];
    // g_key_Buff[17] = g_key_Buff[23];

    /* 199 - 220 v2.0版本更改正式使用 */
    if (g_key_Buff[13] <= 85) {
        g_key_Buff[14] = 1;
        g_key_Buff[15] = 0;
    }else if (g_key_Buff[13] >= 92) {
        g_key_Buff[14] = 0;
        g_key_Buff[15] = 1;
    }else {
        g_key_Buff[14] = 0;
        g_key_Buff[15] = 0;
    }
    // g_key_Buff[13] = g_key_Buff[13] - 88;

    if (g_key_Buff[29] <= 85) {
        g_key_Buff[30] = 1;
        g_key_Buff[31] = 0;
    }else if (g_key_Buff[29] >= 92) {
        g_key_Buff[30] = 0;
        g_key_Buff[31] = 1;
    }else {
        g_key_Buff[30] = 0;
        g_key_Buff[31] = 0;
    }
    // g_key_Buff[29] = g_key_Buff[29] - 88;

    for (uint8_t i = 0; i < 13; i++) {
        cur_signal |=(uint32_t)g_key_Buff[i] << i;
    }
    for (uint8_t i = 14; i < 29; i++) {
        cur_signal |=(uint32_t)g_key_Buff[i] << i;
    }
    for (uint8_t i = 30; i < 32; i++) {
        cur_signal |=(uint32_t)g_key_Buff[i] << i;
    }

    if (cur_signal != ber_signal)
        g_key_Buff[16] = 1;
    else
        g_key_Buff[16] = 0;
    ber_signal = cur_signal;
}

/**
 * @brief       接收数据处理函数
 * @param       _RxHeader  ：FDCAN Rx header structure definition
 * @retval      _RxMsg     ：Data
 */
static int GetFdCanBoot(FDCAN_RxHeaderTypeDef *_RxHeader,uint8_t *_RxMsg)
{
    /* 解析数据包 */
    uint16_t size = _RxHeader->DataLength;
    if(5 == size)  /* DLC==5，数据长度为5 */
    {
        /* 判断数据头帧 */
        uint8_t t_head[2] = {0};
        t_head[0] = _RxMsg[0];
        t_head[1] = _RxMsg[1];
        if ((t_head[0] != 0x02) || (t_head[1] != 0xfd))
        {
            return no_begin;
        }

        /* 判断数据尾帧 */
        uint8_t t_tail[2] = {0};
        t_tail[0] = _RxMsg[3];
        t_tail[1] = _RxMsg[4];
        if ((t_tail[0] != 0x03) || (t_tail[1] != 0xfc))
        {
            return no_end;
        }

        /* 判断指令id */
        uint8_t t_id = _RxMsg[2];
        if (t_id == 0x01) {
            /* 数据接收成功，继续发送下一字段 */
            g_UpgradeBegin = 1;
            return enough;

        }else if (t_id == 0x02) {
            /* 数据发送失败，根据当前状态选择模式 */
            return no_enough;
        }else if (t_id == 0x03) {
            return hand_shake;
        }else if (t_id == 0x04) {
            return end_data;
        }
    }
    else
    {
        return no_enough;
    }
    return enough;
}
/**
 * @brief       接收数据处理函数
 * @param       RxHeader  ：FDCAN Rx header structure definition
 * @retval      TXmessage ：Data
 */
static uint8_t GetFDcanData(FDCAN_RxHeaderTypeDef *_RxHeader,uint8_t *_RxMsg)
{
   if (0x100 < _RxHeader->Identifier)     /* 正常处理数据流程 */
   {
       AnalyseData(_RxHeader,_RxMsg,g_lAngel_Buff,g_rAngel_Buff,g_key_Buff);
   }
   else                                /* bootloader升级流程 */
   {
       int ret = -1;
       ret = GetFdCanBoot(_RxHeader,_RxMsg);
       /* 获取数据 */
       if (enough == ret)               /* 从机数据接收成功 */
       {
           g_UpgradeBegin = 1;
       }
       else if (hand_shake == ret)      /* 从机握手成功 */
       {
           g_Shakehands = 1;
       }
       else if (end_data == ret) {
           g_EndData = 1;

           /* 获取id号 */
           uint32_t id = 0;
           id = _RxHeader->Identifier;
           if (id == 0x11) {
               g_AllUpgrade |= 0x0001;
           }else if (id == 0x12) {
               g_AllUpgrade |= 0x0002;
           }else if (id == 0x13) {
               g_AllUpgrade |= 0x0004;
           }else if (id == 0x14) {
               g_AllUpgrade |= 0x0008;
           }else if (id == 0x15) {
               g_AllUpgrade |= 0x0010;
           }else if (id == 0x16) {
               g_AllUpgrade |= 0x0020;
           }else if (id == 0x17) {
               g_AllUpgrade |= 0x0040;
           }else if (id == 0x31) {
               g_AllUpgrade |= 0x0080;
           }else if (id == 0x32) {
               g_AllUpgrade |= 0x0100;
           }else if (id == 0x33) {
               g_AllUpgrade |= 0x0200;
           }else if (id == 0x34) {
               g_AllUpgrade |= 0x0400;
           }else if (id == 0x35) {
               g_AllUpgrade |= 0x0800;
           }else if (id == 0x36) {
               g_AllUpgrade |= 0x1000;
           }else if (id == 0x37) {
               g_AllUpgrade |= 0x2000;
           }
       }
       else if (no_begin == ret || no_end == ret || no_enough == ret) {
           g_ReceiveNg = 1;
       }
   }
}
