#include "bsp_spi.h"

/* spi初始化 */
void spi_init(ECSPI_Type *base)
{
    base->CONREG = 0;       /* 清零 */
    base->CONREG |= (1 << 0) | (1 << 3) | (1 << 4) | (7 << 20);
    base->CONFIGREG = 0;
    base->PERIODREG = 0x2000;

    /* SPI时钟 ICM20608的SPI最高8MHz 将SPI CLK=6MHz*/
    base->CONREG &= ~((0xf << 12) | (0xf << 8));     /* 先将bit15:12 bit11:8清零 */
    base->CONREG |= (9 << 12);                       /* 1级10分频，60MHz/10 = 6MHz */ 
}

/* SPI发送/接收函数 */
unsigned char spich0_readwrite_byte(ECSPI_Type *base, unsigned char txdata)
{
    uint32_t spirxdata = 0;
    uint32_t spitxdata = txdata;

    /* 选择通道0 */
    base->CONREG &= ~(3 << 18); /* 清零 */

    /* 数据发送 */
    while((base->STATREG & (1 << 0)) == 0);
    base->TXDATA = spitxdata;

    /* 数据接收 */
    while((base->STATREG & (1 << 3)) == 0);
    spirxdata = base->RXDATA;

    return spirxdata;
}