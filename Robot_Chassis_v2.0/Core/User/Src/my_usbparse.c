
/**
 * @file    my_usbparse.c
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

#include "../Inc/my_usbparse.h"

uint8_t g_ParseResults = 0;
uint8_t g_OutManner = 0;
uint16_t g_AllUpgrade = 0;
uint8_t g_Shakehands = 0;
uint8_t g_UpgradeBegin = 0;
uint8_t g_ReceiveNg = 0;
uint8_t g_EndData = 0;

uint32_t g_id = 0x99;
/**
 * @brief       接收命令解析函数
 * @param       **
 * @retval      **
 */
void ReceiveDisassembly() {
    /* 判断是否够包头大小 */
    uint16_t Size = RingBuffGetLen(&Usb_RingBuff_t);
    if (Size < 5) {
        /* 不够长度 */
        g_ParseResults = no_enough;
    } else {
        /* 判断帧头是否正确 */
        uint8_t temp_data[4];
        RingBuffReadNByte(&Usb_RingBuff_t, temp_data, 4);
        if ((temp_data[0] == 0xEB) && (temp_data[1] == 0x09)) {
            // 数据总长度
            uint8_t sum_size = temp_data[3] + 4 + 2;
            // 3.剩余字节是否够大
            if (Size < sum_size) {
                g_ParseResults = no_enough;
            } else {
                // 4. 判断帧尾
                if ((RingBuffGetIndexItem(&Usb_RingBuff_t, sum_size - 1) == 0x0A) &&
                    (RingBuffGetIndexItem(&Usb_RingBuff_t, sum_size - 2) == 0x0D)) {
                    // 取出一个完整一包数据,不包括帧头帧尾
                    RingBuffRemoveNByte(&Usb_RingBuff_t, 2);
                    uint8_t pack_data[256];
                    RingBuffGetNByte(&Usb_RingBuff_t, pack_data, sum_size - 4);
                    RingBuffRemoveNByte(&Usb_RingBuff_t, 2);
                    //  数据包解析
                    USBReceiveDataProcess(pack_data);
                    g_ParseResults = enough;
                } else {
                    // 找不到帧尾，移除帧头，重新寻找新的一帧
                    RingBuffRemoveNByte(&Usb_RingBuff_t, 2);
                    RingBuffMoveRight();
                    g_ParseResults = data_error;
                }
            }
        } else {
            RingBuffMoveRight();
            g_ParseResults = no_begin;
        }
    }
}
/**
 * @brief       环形队列移动函数，用来移动至有帧头的位置
 * @param       **
 * @retval      **
 */
void RingBuffMoveRight()
{
    uint16_t sum_size = RingBuffGetLen(&Usb_RingBuff_t);
    // 数据太少判断
    if (sum_size < 2) {
        return;
    }
    for (size_t i = 0; i < sum_size; i++) {
        uint8_t res = 0;
        uint8_t temp_data[2];
        RingBuffReadNByte(&Usb_RingBuff_t, temp_data, 2);
        if ((temp_data[0] == 0xEB) && (temp_data[1] == 0x09)) {
            // 找到退出
            break;
        } else {
            res = RingBuffRemoveNByte(&Usb_RingBuff_t, 1);
        }
        if (res == 0) {
            // 数据空跳出
            break;
        }
    }
}
/**
 * @brief       改变编码器方向
 * @param       _data : 关键报文
 * @retval      **
 */
static void SetDaisySensorDirection(uint8_t *_data) {
	uint8_t data_direction = 0x11;
    if (_data[0] == 0) {
    	uint32_t ID_l = 0x200 | _data[1];
		CanSendMsg(ID_l, &data_direction , 1);
    } else if (_data[0] == 1) {
    	uint32_t ID_l = 0x210 | _data[1];
    	CanSendMsg(ID_l, &data_direction , 1);
    }
}

/**
 * @brief       改变编码器原点
 * @param       _data : 关键报文
 * @retval      **
 */
static void SetDaisySensorZero(uint8_t *_data) {
	uint8_t data_zero = 0x10;
    if (_data[0] == 0) {
    	uint32_t ID_l = 0x200 | _data[1];
    	CanSendMsg(ID_l, &data_zero , 1);
    } else if (_data[0] == 1) {
    	uint32_t ID_r = 0x210 | _data[1];
		CanSendMsg(ID_r, &data_zero , 1);
    }
}

/**
 * @brief       保存编码器数据
 * @param       _data : 关键报文
 * @retval      **
 */
static void SetDaisySensorFlash(uint8_t *_data)
{
	uint8_t data_flash = 0x01;
	if (_data[0] == 0) {
	    uint32_t ID_l = 0x200 | _data[1];
	    CanSendMsg(ID_l, &data_flash , 1);
	 } else if (_data[0] == 1) {
	    	uint32_t ID_r = 0x210 | _data[1];
			CanSendMsg(ID_r, &data_flash , 1);
	 }
}

/**
 * @brief       更改芯片升级标志位
 * @param       _data : 关键报文
 * @retval      **
 */
static void SetDaisySensorUpgrade(uint8_t *_data)
{
    uint8_t data_upgrade = 0xbb;
    if (_data[0] == 0) {
        uint32_t ID_l = 0x200 | _data[1];
        CanSendMsg(ID_l, &data_upgrade , 1);
    } else if (_data[0] == 1) {
        uint32_t ID_r = 0x210 | _data[1];
        CanSendMsg(ID_r, &data_upgrade , 1);
    }
}

/**
 * @brief       更改全部芯片升级标志位
 * @param       _data : 关键报文
 * @retval      **
 */
static void SetDaisySensorUpgradeall(uint8_t *_data)
{
    uint32_t ID = 0;
    uint8_t data_upgrade = 0xbb;
    for (int j = 1; j < 9; ++j) {
            ID = 0x200 | j;
            CanSendMsg(ID, &data_upgrade , 1);
            HAL_Delay(50);
    }
    HAL_Delay(50);
    for (int j = 1; j < 9; ++j) {
        ID = 0x210 | j;
        CanSendMsg(ID, &data_upgrade , 1);
        HAL_Delay(50);
    }
    HAL_Delay(50);

}

/**
 * @brief       给全部关节芯片升级
 * @param       _data : 关键报文
 * @retval      **
 */
static void CanFlashMoveDataAll(uint8_t *_data)
{
    uint8_t u8_msg[64] = {0};
    uint8_t u8_canid = 0;
    uint8_t u8_rdflash[32] = {0};
    uint8_t u8_kindid[2] = {0x53,0x01};

    /* 获取需要升级的canid */
    if (0 == _data[0])
    {
        u8_canid = _data[1];
    }
    else
    {
        u8_canid = _data[1] | 0x10;
    }

    g_AllUpgrade = 0;

    while(0x3fff != g_AllUpgrade)
    {
        /* 重新进入清除状态 */
        g_Shakehands = 0;
        g_ReceiveNg = 0;
        g_EndData = 0;
        g_UpgradeBegin = 0;

        /* 发送握手包 */
        u8_kindid[1] = 0x01;
        GeneralFrameUPInit(&Boot_Frame,1,u8_kindid,&BootLoader_RingBuff_t);
        BootPack(&Boot_Frame,u8_rdflash,u8_canid);
        AgreementEncapsula(&Boot_Frame,u8_msg);
        CanSendMsg(g_id,u8_msg,FDCAN_DLC_BYTES_48);

        /* 等待接收到握手信息 */
        while(!g_Shakehands) {
         if (1 == g_ReceiveNg) {
             /* 发送握手包 */
             GeneralFrameUPInit(&Boot_Frame,1,u8_kindid,&BootLoader_RingBuff_t);
             BootPack(&Boot_Frame,u8_rdflash,u8_canid);
             AgreementEncapsula(&Boot_Frame,u8_msg);
             CanSendMsg(g_id,u8_msg,FDCAN_DLC_BYTES_48);
             g_ReceiveNg = 0;
         }
        }

        /* 设置app2地址 */
        uint32_t addr = BANK1_FLASH_PAGE(32);

        while(1)
        {
            /* 发送升级固件 */
            u8_kindid[1] = 0x02;
            GeneralFrameUPInit(&Boot_Frame,1,u8_kindid,&BootLoader_RingBuff_t);
            FlashRead(addr,(uint32_t *)u8_rdflash,8);
            BootPack(&Boot_Frame,u8_rdflash,u8_canid);
            AgreementEncapsula(&Boot_Frame,u8_msg);
            CanSendMsg(g_id,u8_msg,FDCAN_DLC_BYTES_48);
            addr += 32;

            /* 判断数据是否发送完成 */
            if (addr > 0x0801FFFF )
            {
                u8_kindid[1] = 0x03;
                GeneralFrameUPInit(&Boot_Frame,1,u8_kindid,&BootLoader_RingBuff_t);
                BootPack(&Boot_Frame,u8_rdflash,u8_canid);
                AgreementEncapsula(&Boot_Frame,u8_msg);
                CanSendMsg(g_id,u8_msg,FDCAN_DLC_BYTES_48);
                u8_canid += 0x01;
                if (0x8 == u8_canid) {
                    u8_canid = 0x11;
                }
                while (0 == g_EndData) {
                    if (1 == g_ReceiveNg) {
                        u8_canid -= 0x01;
                        if (0x10 == u8_canid) {
                            u8_canid = 0x08;
                        }
                        u8_kindid[1] = 0x03;
                        GeneralFrameUPInit(&Boot_Frame,1,u8_kindid,&BootLoader_RingBuff_t);
                        BootPack(&Boot_Frame,u8_rdflash,u8_canid);
                        AgreementEncapsula(&Boot_Frame,u8_msg);
                        CanSendMsg(g_id,u8_msg,FDCAN_DLC_BYTES_48);
                        g_ReceiveNg = 0;
                    }
                }
                break;
            }

            /* 等待允许继续发送升级固件 */
            while(0 == g_UpgradeBegin) {
                if (1 == g_ReceiveNg) {
                    addr -= 32;
                    /* 发送升级固件 */
                    u8_kindid[1] = 0x02;
                    GeneralFrameUPInit(&Boot_Frame,1,u8_kindid,&BootLoader_RingBuff_t);
                    FlashRead(addr,(uint32_t *)u8_rdflash,8);
                    BootPack(&Boot_Frame,u8_rdflash,u8_canid);
                    AgreementEncapsula(&Boot_Frame,u8_msg);
                    CanSendMsg(g_id,u8_msg,FDCAN_DLC_BYTES_48);
                    g_ReceiveNg = 0;
                }
            }

            /* 清除写入标志 */
            g_UpgradeBegin = 0;
        }
        /* 清除写入标志 */
        g_UpgradeBegin = 0;
    }
    /* 清除全部升级完成标志位 */
    g_AllUpgrade = 0;
}

/**
 * @brief       给单个芯片升级
 * @param       _data : 关键报文
 * @retval      **
 */
static void CanFlashMoveData(uint8_t *data)
{

    uint8_t u8_msg[64] = {0};
    uint8_t u8_canid = 0;
    uint8_t u8_rdflash[32] = {0};
    uint8_t u8_kindid[2] = {0x53,0x01};

    /* 获取需要升级的canid */
    if (0 == data[0])
    {
        u8_canid = data[1];
    }
    else
    {
        u8_canid = data[1] | 0x10;
    }

    while(1)
    {
        /* 重新进入清除状态 */
        g_Shakehands = 0;
        g_ReceiveNg = 0;
        g_EndData = 0;
        g_UpgradeBegin = 0;

        /* 发送握手包 */
        GeneralFrameUPInit(&Boot_Frame,1,u8_kindid,&BootLoader_RingBuff_t);
        BootPack(&Boot_Frame,u8_rdflash,u8_canid);
        AgreementEncapsula(&Boot_Frame,u8_msg);
        CanSendMsg(g_id,u8_msg,FDCAN_DLC_BYTES_48);

        /* 等待接收到握手信息 */
        while(!g_Shakehands) {
            if (1 == g_ReceiveNg) {
                /* 发送握手包 */
                GeneralFrameUPInit(&Boot_Frame,1,u8_kindid,&BootLoader_RingBuff_t);
                BootPack(&Boot_Frame,u8_rdflash,u8_canid);
                AgreementEncapsula(&Boot_Frame,u8_msg);
                CanSendMsg(g_id,u8_msg,FDCAN_DLC_BYTES_48);
                g_ReceiveNg = 0;
            }
        }

        /* 设置app2地址 */
        uint32_t addr = BANK1_FLASH_PAGE(32);

        while(1)
        {
            /* 发送升级固件 */
            u8_kindid[1] = 0x02;
            GeneralFrameUPInit(&Boot_Frame,1,u8_kindid,&BootLoader_RingBuff_t);
            FlashRead(addr,(uint32_t *)u8_rdflash,8);
            BootPack(&Boot_Frame,u8_rdflash,u8_canid);
            AgreementEncapsula(&Boot_Frame,u8_msg);
            CanSendMsg(g_id,u8_msg,FDCAN_DLC_BYTES_48);
            addr += 32;

            /* 判断数据是否发送完成 */
            if (addr > 0x0801FFFF )
            {
                u8_kindid[1] = 0x03;
                GeneralFrameUPInit(&Boot_Frame,1,u8_kindid,&BootLoader_RingBuff_t);
                BootPack(&Boot_Frame,u8_rdflash,u8_canid);
                AgreementEncapsula(&Boot_Frame,u8_msg);
                CanSendMsg(g_id,u8_msg,FDCAN_DLC_BYTES_48);

                while (0 == g_EndData) {
                    if (1 == g_ReceiveNg) {
                        u8_kindid[1] = 0x03;
                        GeneralFrameUPInit(&Boot_Frame,1,u8_kindid,&BootLoader_RingBuff_t);
                        BootPack(&Boot_Frame,u8_rdflash,u8_canid);
                        AgreementEncapsula(&Boot_Frame,u8_msg);
                        CanSendMsg(g_id,u8_msg,FDCAN_DLC_BYTES_48);
                        g_ReceiveNg = 0;
                    }
                }
                return;
            }

            /* 等待允许继续发送升级固件 */
            while(!g_UpgradeBegin) {
                if (1 == g_ReceiveNg) {
                    addr -= 32;
                    /* 发送升级固件 */
                    u8_kindid[1] = 0x02;
                    GeneralFrameUPInit(&Boot_Frame,1,u8_kindid,&BootLoader_RingBuff_t);
                    FlashRead(addr,(uint32_t *)u8_rdflash,8);
                    BootPack(&Boot_Frame,u8_rdflash,u8_canid);
                    AgreementEncapsula(&Boot_Frame,u8_msg);
                    CanSendMsg(g_id,u8_msg,FDCAN_DLC_BYTES_48);
                    g_ReceiveNg = 0;
                }
            }

            /* 清除写入标志 */
            g_UpgradeBegin = 0;
        }

    }
}

/**
 * @brief       分析协议
 * @param       _pdata : 关键报文
 * @retval      **
 */
void USBReceiveDataProcess(uint8_t *_pdata)
{
    // ID位
    uint8_t id = _pdata[0];
    // 数据长度位
    uint8_t len_data = _pdata[1];
    // 协议数据起始位置
    uint8_t *data_start = &_pdata[2];
    switch (id) {
        case SET_ZERO:                               /* 编码器原点更改 */
            SetDaisySensorZero(data_start);
            break;
        case SET_DIRECTION:                          /* 编码器方向更改 */
            SetDaisySensorDirection(data_start);
            break;
        case CHANGE_DATA:
            ChangeData();
            break;
        case FLASH_STORE:                            /* flash保存 */
        	SetDaisySensorFlash(data_start);
        	break;
        case UPGRADEALL:                             /* 更改全部芯片升级标志位 */
            SetDaisySensorUpgradeall(data_start);
            break;
        case UPGRADE:                                /* 升级指定芯片 */
            CanFlashMoveData(data_start);
            break;
        case UPGRADESIGN:                            /* 更改芯片升级标志位 */
            SetDaisySensorUpgrade(data_start);
            break;
        case UPGRADESIGNALL:                         /* 升级全部芯片 */
            CanFlashMoveDataAll(data_start);
            break;
        case REBOOT:
            NVIC_SystemReset();
            break;
        case TURNOFF:
            TurnOfAllDev();
            break;
        case TURNON:
            TurnOnAllDev();
            break;
        default:
            break;
    }
}

/**
 * @brief       更换输出方式
 * @param       **
 * @retval      **
 */
void ChangeData() {
    if (g_OutManner == 0) {
        g_OutManner = 1;
    } else {
        g_OutManner = 0;
    }
}

/**
 * @brief       关闭所有设备
 * @param       **
 * @retval      **
 */
void TurnOfAllDev() {
    uint8_t data_flash = 0x55;
    for (int i = 1; i < 9; i++) {
       CanSendMsg(0x200 + i,&data_flash,FDCAN_DLC_BYTES_1);
        HAL_Delay(10);
    }
    for (int i = 1; i < 9; i++) {
        CanSendMsg(0x210 + i,&data_flash,FDCAN_DLC_BYTES_1);
        HAL_Delay(10);
    }
}

/**
 * @brief       打开所有设备
 * @param       **
 * @retval      **
 */
void TurnOnAllDev() {
    uint8_t data_flash = 0x56;
    for (int i = 1; i < 9; i++) {
        CanSendMsg(0x200 + i,&data_flash,FDCAN_DLC_BYTES_1);
        HAL_Delay(10);
    }
    for (int i = 1; i < 9; i++) {
        CanSendMsg(0x210 + i,&data_flash,FDCAN_DLC_BYTES_1);
        HAL_Delay(10);
    }
}

