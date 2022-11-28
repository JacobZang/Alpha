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
	unsigned char index = 0;
	unsigned int color;
	static unsigned char state = 0;
	
	int_init(); 				/* 初始化中断 */
	imx6u_clkinit();			/* 初始化系统时钟 */
	delay_init();				/* 初始化延时 */
	uart_init();				/* 初始化uart1 */
	clk_enable();				/* 使能所有的时钟 */
	led_init();					/* 初始化led */
	beep_init();				/* 初始化beep */
	key_init();					/* 初始化按键 */
	lcd_init();					/* 初始化屏幕 */

	tftlcd_dev.forecolor = LCD_RED;
	lcd_show_string(10, 40, 260, 32, 32, (char *)"Alpha IMX6ULL");
	lcd_show_string(10, 80, 260, 24, 24, (char *)"RGBLCD TEST");
	lcd_show_string(10, 110, 260, 16, 16, (char *)"ATOM&ALIENTEK");
	lcd_show_string(10, 130, 260, 12, 12, (char *)"2022/11/28");

	while(1)			
	{	
		index++;
		lcd_clear(backcolor[index]);
		lcd_show_string(10, 40, 260, 32, 32, (char *)"Alpha IMX6ULL");
		lcd_show_string(10, 80, 260, 24, 24, (char *)"RGBLCD TEST");
		lcd_show_string(10, 110, 260, 16, 16, (char *)"ATOM&ALIENTEK");
		lcd_show_string(10, 130, 260, 12, 12, (char *)"2022/11/30");
		if (index == 10)
			index = 0;
		state = !state;
		led_switch(LED0, state);
		delayms(1000);
	}

	return 0;
}
