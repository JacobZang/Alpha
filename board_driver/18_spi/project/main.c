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
#include "bsp_spi.h"
#include "bsp_icm20608.h"

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
	icm20608_init();			/* 初始化icm20608 */

	tftlcd_dev.backcolor = LCD_WHITE;
	tftlcd_dev.forecolor = LCD_RED;
	lcd_show_string(30, 50, 200, 16, 16, (char*)"ALPHA-IMX6UL SPI TEST");    /* 显示字符串 */
	lcd_show_string(30, 70, 200, 16, 16, (char*)"ICM20608 TEST");    /* 显示字符串 */
	lcd_show_string(30, 90, 200, 16, 16, (char*)"ATOM@ALIENTEK");  
	lcd_show_string(30, 110, 200, 16, 16, (char*)"2023/1/23");  

	while(1)
	{
		icm20608_getdata();
		printf("accelerate x=%d\r\n", icm20608_dev.accel_x_adc);
		printf("accelerate y=%d\r\n", icm20608_dev.accel_y_adc);
		printf("accelerate z=%d\r\n", icm20608_dev.accel_z_adc);
		printf("gyro x=%d\r\n", icm20608_dev.gyro_x_adc);
		printf("gyro y=%d\r\n", icm20608_dev.gyro_y_adc);
		printf("gyro z=%d\r\n", icm20608_dev.gyro_z_adc);
		state = !state;
		led_switch(LED0,state);
		delayms(100);	/* 延时0.1秒 */
	}

	return 0;
}
