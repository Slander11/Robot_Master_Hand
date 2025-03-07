
/**
 * @file    my_flash.c
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

#include "../Inc/my_flash.h"

/**
 * @brief       从指定地址读取一个字 (32位数据)
 * @param       _faddr : 读取地址 (此地址必须为4倍数!!)
 * @retval      读取到的数据 (32位)
 */
uint32_t FlashReadWord(uint32_t _faddr)
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
void FlashRead(uint32_t _raddr, uint32_t *_pbuf, uint32_t _length)
{
    uint32_t i;
    for (i = 0; i < _length; i++)
    {
        _pbuf[i] = FlashReadWord(_raddr);        /* 读取4个字节 */
        _raddr += 4;                             /* 偏移4个字节 */
    }
}

/**
 * @brief       获取某个地址所在的flash页.stm32g431cbt6只有512K，即256页
 * @param       _addr   : flash地址
 * @retval      0~127 ,addr所在的bank1页编号
 */
uint8_t GetFlashSector(uint32_t _addr)
{
    for (int i = 0; i < 64; ++i) {
        if (_addr >= BANK1_FLASH_PAGE(i) && _addr < BANK1_FLASH_PAGE(i+1))
            return i + 1;
    }
}

/**
 * @brief       从指定地址开始写入指定长度的数据
 * @param       _waddr  : 起始地址(此地址必须为4的倍数!!)
                _pbuf   : 数据指针
                _length : 字(32位)数(就是要写入的32位数据的个数)
 * @retval      **
 */
void FlashWrite(uint32_t _waddr, uint32_t *_pbuf, uint32_t _length)
{
    FLASH_EraseInitTypeDef FlashEraseInit;
    HAL_StatusTypeDef FlashStatus = HAL_OK;
    uint32_t SectorError = 0;
    uint32_t addrx = 0;
    uint32_t endaddr = 0;

    if (_waddr < STM32_FLASH_BASE || _waddr % 32 ||      /* 写入地址小于 STM32_FLASH_BASE, 或不是32的整数倍, 非法. */
        _waddr > (STM32_FLASH_BASE + STM32_FLASH_SIZE)) /* 写入地址大于 STM32_FLASH_BASE + STM32_FLASH_SIZE, 非法. */
    {
        return;
    }

    addrx = _waddr;                /* 写入的起始地址 */
    endaddr = _waddr + _length * 4; /* 写入的结束地址 */

    HAL_FLASH_Unlock();           /* 解锁 */
    if (addrx < 0X1FF00000)
    {
        while (addrx < endaddr) /* 扫清一切障碍.(对非FFFFFFFF的地方,先擦除) */
        {
            if (FlashReadWord(addrx) != 0XFFFFFFFF) /* 有非0XFFFFFFFF的地方,要擦除这个页 */
            {
                FlashEraseInit.Banks = FLASH_BANK_1;
                FlashEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;                /* 擦除类型，页擦除 */
                FlashEraseInit.Page = GetFlashSector(addrx);    /* 要擦除的页 */
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
            _waddr += 32;
            _pbuf += 8;
        }
    }
    HAL_FLASH_Lock(); /* 上锁 */
}


/**
 * @brief       在FLASH 指定位置, 写入指定长度的数据(不擦除写入)
 * @param       _waddr   : 起始地址 (此地址必须为4的倍数!!,否则写入出错!)
 * @param       _pbuf    : 数据指针
 * @param       _length  : 要写入的 字(32位)数(就是要写入的32位数据的个数)
 * @retval      无
 */
void FlashNoEraseWrite(uint32_t _waddr, uint32_t *_pbuf, uint32_t _length)
{
    HAL_StatusTypeDef FlashStatus = HAL_OK;

    uint32_t endaddr = 0;

    HAL_FLASH_Unlock(); /* 解锁 */

    endaddr = _waddr + _length * 4; /* 写入的结束地址 */

    FlashStatus = FLASH_WaitForLastOperation(FLASH_WAITETIME); /* 等待上次操作完成 */

    if (FlashStatus == HAL_OK)
    {
        while (_waddr < endaddr) /* 写数据 */
        {
            if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, _waddr, *_pbuf) != HAL_OK) /* 写入数据 */
            {
                break; /* 写入异常 */
            }
            _waddr += 32;
            _pbuf += 4;
        }
    }

    HAL_FLASH_Lock(); /* 上锁 */
}

/**
 * @brief       擦除n个页
 * @param       _waddr : 起始地址
 * @retval      0 成功  1 失败
 */
uint8_t FalshErasePage(uint32_t _waddr)
{
    uint32_t FirstPage = 0, NbOfPage = 0;
    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t SECTORError = 0;

    /* 解锁 */
    HAL_FLASH_Unlock();

    /* 获取此地址所在的扇区 */
    FirstPage = GetFlashSector(_waddr);

    /* 擦除的页 */
    NbOfPage = 64 - FirstPage;

    EraseInitStruct.Banks = FLASH_BANK_1;
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;              /* 擦除类型，页擦除 */
    EraseInitStruct.Page = FirstPage;                               /* 要擦除的页 */
    EraseInitStruct.NbPages = NbOfPage;                             /* 一次只擦除一页 */
    if (HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return 1;  /* 失败 */
    }
    FLASH_WaitForLastOperation(FLASH_WAITETIME);


    /* 上锁 */
    HAL_FLASH_Lock();

    return 0;   /* 成功 */
}