#include "bsp_icm20608.h"
#include "bsp_spi.h"
#include "stdio.h"
#include "bsp_delay.h"

struct icm20608_dev_struc icm20608_dev; /* icm20608设备 */

/* 初始化ICM20608 */
unsigned char icm20608_init(void)
{
    unsigned char regval = 0;
    /* 1.SPI引脚初始化 */
    IOMUXC_SetPinMux(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK,0);       /* 复用为ECSPI3_SCLK */
	IOMUXC_SetPinMux(IOMUXC_UART2_CTS_B_ECSPI3_MOSI,0);         /* 复用为ECSPI3_MOSI */
	IOMUXC_SetPinMux(IOMUXC_UART2_RTS_B_ECSPI3_MISO,0);         /* 复用为ECSPI3_MISO */

	IOMUXC_SetPinConfig(IOMUXC_UART2_RX_DATA_ECSPI3_SCLK,0x10b0);   /* 复用为I2C1_SDA */
	IOMUXC_SetPinConfig(IOMUXC_UART2_CTS_B_ECSPI3_MOSI,0x10b0);     /* 复用为I2C1_SDA */
	IOMUXC_SetPinConfig(IOMUXC_UART2_RTS_B_ECSPI3_MISO,0x10b0);     /* 复用为I2C1_SDA */
    
    /* 片选引脚初始化 */
    IOMUXC_SetPinMux(IOMUXC_UART2_TX_DATA_GPIO1_IO20,0);
	IOMUXC_SetPinConfig(IOMUXC_UART2_TX_DATA_GPIO1_IO20,0x10b0);     /* 复用为I2C1_SDA */
	
    gpio_pin_config_t cs_config;
    cs_config.direction = kGPIO_DigitalOutput;
    cs_config.outputLogic = 0;
	gpio_init(GPIO1, 20, &cs_config);

    /* 2.SPI控制器初始化 */
    spi_init(ECSPI3);

    icm20608_write_reg(ICM20_PWR_MGMT_1, 0x80);		/* 复位，复位后为0x40,睡眠模式 			*/
	delayms(50);
	icm20608_write_reg(ICM20_PWR_MGMT_1, 0x01);		/* 关闭睡眠，自动选择时钟 					*/
	delayms(50);

    /* 3.ICM20608初始化 */
    regval = icm20608_read_reg(ICM20_WHO_AM_I);
    printf("icm20608id = %#X\r\n", regval);
    if((regval != ICM20608G_ID) && (regval != ICM20608D_ID))
    {
        return 1;
    }
    icm20608_write_reg(ICM20_SMPLRT_DIV, 0x00); 	/* 输出速率是内部采样率	*/
	icm20608_write_reg(ICM20_GYRO_CONFIG, 0x18); 	/* 陀螺仪±2000dps量程 */
	icm20608_write_reg(ICM20_ACCEL_CONFIG, 0x18); 	/* 加速度计±16G量程 */
	icm20608_write_reg(ICM20_CONFIG, 0x04); 		/* 陀螺仪低通滤波BW=20Hz */
	icm20608_write_reg(ICM20_ACCEL_CONFIG2, 0x04); 	/* 加速度计低通滤波BW=21.2Hz */
	icm20608_write_reg(ICM20_PWR_MGMT_2, 0x00); 	/* 打开加速度计和陀螺仪所有轴 */
	icm20608_write_reg(ICM20_LP_MODE_CFG, 0x00); 	/* 关闭低功耗 */
	icm20608_write_reg(ICM20_FIFO_EN, 0x00);		/* 关闭FIFO	*/
    return 0;
}

/* icm20608 SPI接口读数据 */
unsigned char icm20608_read_reg(unsigned char reg)
{
    unsigned char reg_val = 0;

    reg |= 0x80;            /* 地址的bit7置1，表示读取数据 */

    ICM20608_CSN(0);        /* 片选拉低 */

    spich0_readwrite_byte(ECSPI3, reg);     /* 发送要读取的寄存器地址 */
    reg_val = spich0_readwrite_byte(ECSPI3, 0xff);  /* 从机返回寄存器数据 */

    ICM20608_CSN(1);        /* 片选拉高，停止访问 */

    return reg_val;
}

/* icm20608 SPI接口写数据 */
void icm20608_write_reg(unsigned char reg, unsigned char value)
{
    reg &= ~0x80;            /* 地址的bit7置0，表示写数据 */

    ICM20608_CSN(0);        /* 片选拉低 */

    spich0_readwrite_byte(ECSPI3, reg);     /* 发送要读取的寄存器地址 */
    spich0_readwrite_byte(ECSPI3, value);  /* 发送写入的数据 */

    ICM20608_CSN(1);        /* 片选拉高，停止访问 */
}

/* 读取多个寄存器的值 */
void icm20608_read_len(unsigned char reg, unsigned char *buf, unsigned char len)
{
    unsigned i = 0;

    reg |= 0x80;            /* 地址的bit7置1，表示读取数据 */

    ICM20608_CSN(0);        /* 片选拉低 */

    spich0_readwrite_byte(ECSPI3, reg);     /* 发送要读取的寄存器地址 */
    for(i = 0; i < len; i++)
    {
        buf[i] = spich0_readwrite_byte(ECSPI3, 0xff);  /* 从机返回寄存器数据 */
    }

    ICM20608_CSN(1);        /* 片选拉高，停止访问 */
}

/* 获取ICM20608内部数据 */
void icm20608_getdata(void)
{
    unsigned char data[14];
    icm20608_read_len(ICM20_ACCEL_XOUT_H, data, 14);

    /* ADC传感器数据 */
    icm20608_dev.accel_x_adc = (signed short)((data[0] << 8) | data[1]);
    icm20608_dev.accel_y_adc = (signed short)((data[2] << 8) | data[3]); 
	icm20608_dev.accel_z_adc = (signed short)((data[4] << 8) | data[5]); 
	icm20608_dev.temp_adc    = (signed short)((data[6] << 8) | data[7]); 
	icm20608_dev.gyro_x_adc  = (signed short)((data[8] << 8) | data[9]); 
	icm20608_dev.gyro_y_adc  = (signed short)((data[10] << 8) | data[11]);
	icm20608_dev.gyro_z_adc  = (signed short)((data[12] << 8) | data[13]);
}