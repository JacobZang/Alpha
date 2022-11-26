#ifndef _BSP_EPITTIMER_H
#define _BSP_EPITTIMER_H

#include "imx6ull.h"


/* 函数声明 */
void epit1_init(unsigned int frac, unsigned int value);
void epit1_irqhandler(void);

#endif
