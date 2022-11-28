#include "bsp_key.h"
#include "bsp_gpio.h"
#include "bsp_delay.h"
/*
 * @description	: 初始化按键
 * @param 		: 无
 * @return 		: 无
 */
void key_init(void)
{	
	gpio_pin_config_t key_config;
	IOMUXC_SetPinMux(IOMUXC_UART1_CTS_B_GPIO1_IO18,0);
	IOMUXC_SetPinConfig(IOMUXC_UART1_CTS_B_GPIO1_IO18,0xF080);
	key_config.direction = kGPIO_DigitalInput;
	gpio_init(GPIO1,18, &key_config);
	
}

/*
 * @description	: 获取按键值 
 * @param 		: 无
 * @return 		: 0 没有按键按下，其他值:对应的按键值
 */
int key_getvalue(void)
{
	int ret = 0;
	static unsigned char release = 1; /* 按键松开 */ 

	if((release==1)&&(gpio_pinread(GPIO1, 18) == 0)) 		/* KEY0 	*/
	{	
		delay(10);		/* 延时消抖 		*/
		release = 0;	/* 标记按键按下 */
		if(gpio_pinread(GPIO1, 18) == 0)
			ret = KEY0_VALUE;
	}
	else if(gpio_pinread(GPIO1, 18) == 1)
	{
		ret = 0;
		release = 1; 	/* 标记按键释放 */
	}

	return ret;	
}
