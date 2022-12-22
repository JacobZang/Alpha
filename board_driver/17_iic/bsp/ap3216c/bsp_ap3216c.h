#ifndef __BSP_AP3216C_H
#define __BSP_AP3216C_H

#include "imx6ull.h"

#define AP3216C_ADDR        0x1E
/* 寄存器地址 */
#define AP3216C_SYSTEMCONG  0x00
#define AP3216C_INTSTATUS   0x01
#define AP3216C_INTCLEAR    0x02
#define AP3216C_IRDATALOW   0x0A
#define AP3216C_IRDATAHIGH  0x0B
#define AP3216C_ALSDATALOW  0x0C
#define AP3216C_ALSDATAHIGH 0x0D
#define AP3216C_PSDATALOW   0x0E
#define AP3216C_PSDATAHIGH  0x0F

void ap3216c_init(void);
unsigned char ap3216c_readonebyte(unsigned char addr, unsigned char reg);
unsigned char ap3216c_writeonebyte(unsigned char addr, unsigned char reg, unsigned char data);
void ap3216c_readdata(unsigned short *ir, unsigned short *ps, unsigned short *als);

#endif // !__BSP_AP3216C_H