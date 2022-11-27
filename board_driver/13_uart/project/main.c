#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_int.h"
#include "bsp_keyfilter.h"
#include "bsp_uart.h"
#include "stdio.h"

/*
 * @description	: main函数
 * @param 		: 无
 * @return 		: 无
 */
int main(void)
{
	int a,b;
	
	int_init(); 				/* 初始化中断 */
	imx6u_clkinit();			/* 初始化系统时钟 */
	delay_init();				/* 初始化延时 */
	uart_init();				/* 初始化uart1 */
	clk_enable();				/* 使能所有的时钟 */
	led_init();					/* 初始化led */
	beep_init();				/* 初始化beep */
	key_init();					/* 初始化按键 */

	while(1)			
	{	
		printf("请输入两个整数，使用空格隔开");
		scanf("%d %d", &a,&b);
		printf("\r\n 数据%d+%d=%d\r\n", a, b, a+b);
	}

	return 0;
}
