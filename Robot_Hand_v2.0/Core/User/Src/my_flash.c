
/**
 * @file    my_flash.c
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

#include "../Inc/my_flash.h"

INTERN_FLASH flash_param;

/**
 * @brief       从指定地址读取一个字 (32位数据)
 * @param       _faddr : 读取地址 (此地址必须为4倍数!!)
 * @retval      读取到的数据 (32位)
 */
static uint32_t ReadFlashWord(uint32_t _faddr)
{
    return *(volatile uint32_t *)_faddr;
}

/**
 * @brief       从指定地址开始读出指定长度的数据
 * @param       raddr : 起始地址
                pbuf  : 数据指针
                length: 要读取的字(32位)数,即4个字节的整数倍
 * @retval      **
 */
void ReadFlashSpecifyLen(uint32_t _raddr, uint32_t *_pbuf, uint32_t _length)
{
    uint32_t i;
    for (i = 0; i < _length; i++)
    {
        _pbuf[i] = ReadFlashWord(_raddr);     /* 读取4个字节 */
        _raddr += 4;                                /* 偏移4个字节 */
    }
}

/**
 * @brief       从指定地址开始写入指定长度的数据
 * @param       waddr  : 起始地址(此地址必须为4的倍数!!)
                pbuf   : 数据指针
                length : 字(32位)数(就是要写入的32位数据的个数)
 * @retval      **
 */
void WriteFlashData(uint32_t _waddr, uint64_t *_pbuf, uint32_t _length)
{
    FLASH_EraseInitTypeDef FlashEraseInit;
    HAL_StatusTypeDef FlashStatus = HAL_OK;
    uint32_t SectorError = 0;
    uint32_t addrx = 0;
    uint32_t endaddr = 0;
    uint32_t pagepos = 0; // 页位置
    uint32_t offset  = 0; // Address在FLASH中的偏移

    addrx = _waddr;                /* 写入的起始地址 */
    endaddr = _waddr + _length * 4; /* 写入的结束地址 */

    /* 计算偏移地址 */
    offset = _waddr - STM32FLASH_BASE;

    /* 计算当前页位置 */
    pagepos = offset / STM32FLASH_PAGE_SIZE;

    __set_PRIMASK(1);
    HAL_FLASH_Unlock();           /* 解锁 */
    if (addrx < 0X1FF00000)
    {
        while (addrx < endaddr) /* 扫清一切障碍.(对非FFFFFFFF的地方,先擦除) */
        {
            if (ReadFlashWord(addrx) != 0XFFFFFFFF) /* 有非0XFFFFFFFF的地方,要擦除这个页 */
            {
                FlashEraseInit.Banks = FLASH_BANK_1;
                FlashEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;                /* 擦除类型，页擦除 */
                FlashEraseInit.Page = pagepos;    /* 要擦除的页 */
                FlashEraseInit.NbPages = 1;                                     /* 一次只擦除一页 */
                if (HAL_FLASHEx_Erase(&FlashEraseInit, &SectorError) != HAL_OK)
                {
                    break; /* 发生错误了 */
                }
            }
            else
            {
                addrx += 4;
            }
            FLASH_WaitForLastOperation(FLASH_WAITETIME); /* 等待上次操作完成 */
        }
    }
    FlashStatus = FLASH_WaitForLastOperation(FLASH_WAITETIME); /* 等待上次操作完成 */
    if (FlashStatus == HAL_OK)
    {
        while (_waddr < endaddr) /* 写数据 */
        {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, _waddr, (uint64_t)*_pbuf) != HAL_OK) /* 写入数据 */
            {
                break; /* 写入异常 */
            }
            _waddr += 8;
            _pbuf ++;
        }
    }
    HAL_FLASH_Lock(); /* 上锁 */
    __set_PRIMASK(0);
}


/**
 * @brief       设备关键信息flash默认值初始化
 * @param       **
 * @retval      **
 */
void InitFlash(void)
{
    // 变量初始化
    flash_param.upgrade   = 0xaaaaaaaa;
    flash_param.version   = 0x0;
    flash_param.is_enable = 0x11;
    flash_param.can_id    = 0x0;
    flash_param.freq_can  = 0x2;
    flash_param.direction = 0x1;
    flash_param.zero      = 0x00;
    flash_param.iap_id    = 0x0;
    // 判断是否初始化
    uint32_t out_data[8]; // 读存放
    ReadFlashSpecifyLen(WRITE_START_ADDR, out_data, 8);
    if (out_data[2] == 0x11) {
        // 已经写入,读取内部变量
        flash_param.can_id    = (uint8_t)out_data[3];
        flash_param.freq_can  = (uint8_t)out_data[4];
        flash_param.direction = (uint8_t)out_data[5];
        flash_param.zero      = (uint16_t)out_data[6];
        flash_param.iap_id    = (uint8_t)out_data[7];
        return;
    }
    // 未写入，写入初始默认值
    WriteFlashDefault(0);
}

/**
 * @brief       写入flash默认值
 * @param       **
 * @retval      **
 */
void WriteFlashDefault(uint8_t mode)
{
    uint32_t out_data[8]; /* 读存放 */
    ReadFlashSpecifyLen(WRITE_START_ADDR, out_data, 8);

    uint32_t in_data[8]; /* 要写入的数据 */
    if (0 == mode)
    {
        in_data[0] = flash_param.upgrade;
        in_data[1] = flash_param.version;
    }
    else if (1 == mode)
    {
        in_data[0] = out_data[0];
        in_data[1] = out_data[1];
    }
    else if(2 == mode)
    {
        in_data[0] = 0xbbbbbbbb;
        in_data[1] = out_data[1];
    }
    else if (3 == mode)
    {
        in_data[0] = 0xaaaaaaaa;
        in_data[1] = out_data[1] + 1;
    }

    in_data[2] = (uint32_t)flash_param.is_enable;
    in_data[3] = (uint32_t)flash_param.can_id;
    in_data[4] = (uint32_t)flash_param.freq_can;
    in_data[5] = (uint32_t)flash_param.direction;
    in_data[6] = (uint32_t)flash_param.zero;
    in_data[7] = (uint32_t)flash_param.iap_id;

    uint64_t w_data[4] = {0};
    for (int i = 0; i < 4; ++i) {
        w_data[i] = ((uint64_t)in_data[2 * i + 1] << 32) | in_data[2 * i];
    }
    WriteFlashData(WRITE_START_ADDR, w_data, 8);
}

/**
 * @brief       获取芯片ID
 * @param       _chipid : 芯片id保存
 * @retval      **
 */
void ReadChipId(uint32_t *_chipId)
{
    *_chipId = *(volatile uint32_t  *)(STM32FLASH_DEVICE_ID);
}

/**
* @brief       升级检查
* @param       **
* @retval      **
*/
void UpgradeFlashExamine(void)
{
    uint32_t t_buffer[8] = {};
    ReadFlashSpecifyLen(WRITE_START_ADDR, t_buffer, 8);
    if (STM32_UPGRADE == t_buffer[0])
    {
        WriteFlashDefault(3);
    }
}
