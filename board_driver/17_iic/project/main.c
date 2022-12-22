#include "bsp_clk.h"
#include "bsp_delay.h"
#include "bsp_led.h"
#include "bsp_beep.h"
#include "bsp_key.h"
#include "bsp_int.h"
#include "bsp_keyfilter.h"
#include "bsp_uart.h"
#include "stdio.h"
#include "bsp_lcd.h"
#include "bsp_lcdapi.h"
#include "bsp_rtc.h"
#include "bsp_ap3216c.h"

/* 背景颜色索引 */
unsigned int backcolor[10] = {
	LCD_BLUE, 		LCD_GREEN, 		LCD_RED, 	LCD_CYAN, 	LCD_YELLOW, 
	LCD_LIGHTBLUE, 	LCD_DARKBLUE, 	LCD_WHITE, 	LCD_LIGHTCYAN, 	LCD_ORANGE

}; 

/*
 * @description	: main函数
 * @param 		: 无
 * @return 		: 无
 */
int main(void)
{
	unsigned short ir,ps,als;
	unsigned char state = OFF;
	
	int_init(); 				/* 初始化中断 */
	imx6u_clkinit();			/* 初始化系统时钟 */
	delay_init();				/* 初始化延时 */
	uart_init();				/* 初始化uart1 */
	clk_enable();				/* 使能所有的时钟 */
	led_init();					/* 初始化led */
	beep_init();				/* 初始化beep */
	key_init();					/* 初始化按键 */
	lcd_init();					/* 初始化屏幕 */
	ap3216c_init();				/* 初始化AP3216C */

	tftlcd_dev.forecolor = LCD_RED;
	lcd_show_string(50, 10, 400, 24, 24, (char*)"ALPHA-IMX6UL IIC TEST");    /* 显示字符串 */
	lcd_show_string(50, 40, 200, 16, 16, (char*)"ATOM@ALIENTEK");  
	lcd_show_string(50, 60, 200, 16, 16, (char*)"2022/12/22");  

	while(1)
	{
		ap3216c_readdata(&ir, &ps, &als);
		printf("ir=%d, ps=%d, als=%d\r\n", ir, ps, als);
		state = !state;
		led_switch(LED0,state);
		delayms(1000);	/* 延时一秒 */
	}

	return 0;
}
