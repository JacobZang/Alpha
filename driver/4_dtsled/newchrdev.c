#include "asm-generic/errno-base.h"
#include "asm-generic/int-ll64.h"
#include "asm/io.h"
#include "asm/uaccess.h"
#include "linux/cdev.h"
#include "linux/device.h"
#include "linux/err.h"
#include "linux/export.h"
#include "linux/kdev_t.h"
#include "linux/of.h"
#include "linux/of_address.h"
#include "linux/ratelimit.h"
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/ide.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>

#define NEWCHRDEV_CNT   1
#define NEWCHRDEV_NAME "dts_led" 

#define LEDON  0
#define LEDOFF 1

static void __iomem *IMX6U_CCM_CCGR1;
static void __iomem *SW_MUX_GPIO1_IO03;
static void __iomem *SW_PAD_GPIO1_IO03;
static void __iomem *GPIO1_DR;
static void __iomem *GPIO1_GDIR;

struct newchrled_dev
{
    dev_t devid;
    struct cdev cdev;
    struct class *class;
    struct device *device;
    int major;
    int minor;
    struct device_node *nd;
};

struct newchrled_dev newchrled;

void led_switch(u8 sta)
{
    u32 val = 0;
    if(sta == LEDON)
    {
        val = readl(GPIO1_DR);
        val &= ~(1 << 3);
        writel(val, GPIO1_DR);
    }
    else if(sta == LEDOFF)
    {
        val = readl(GPIO1_DR);
        val |= (1 << 3);
        writel(val, GPIO1_DR);
    }
}

static int led_open(struct inode *inode, struct file *file)
{
    printk("led_open\n");
    file->private_data = &newchrled;
    return 0;
}

static ssize_t led_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    printk("led_read\n");
    return 0;
}

static ssize_t led_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    int retval = 0;
    u8 databuf[1];
    u8 ledstate;

    retval = copy_from_user(databuf, buf, len);
    if(retval < 0)
    {
        printk("write driver error\n");
        return -EFAULT;
    }

    ledstate = databuf[0];

    if(ledstate == LEDON)
    {
        led_switch(LEDON);
    }
    else if(ledstate == LEDOFF)
    {
        led_switch(LEDOFF);
    }

    printk("led_write\n");
    return 0;
}

static int led_release(struct inode *inode, struct file *file)
{
    printk("led_release\n");
    return 0;
}

static struct file_operations newchrled_fops = 
{
    .owner   = THIS_MODULE,
    .open    = led_open,
    .read    = led_read,
    .write   = led_write,
    .release = led_release,
};

void led_hd_init(void)
{
    u32 val = 0;
    /* 1、寄存器地址映射 */
    

    /* 2、使能 GPIO1 时钟 */
    val = readl(IMX6U_CCM_CCGR1);
    val &= ~(3 << 26); /* 清除以前的设置 */
    val |= (3 << 26); /* 设置新值 */
    writel(val, IMX6U_CCM_CCGR1);

    /* 3、设置 GPIO1_IO03 的复用功能，将其复用为
    * GPIO1_IO03，最后设置 IO 属性。
    */
    writel(5, SW_MUX_GPIO1_IO03);

    /* 寄存器 SW_PAD_GPIO1_IO03 设置 IO 属性 */
    writel(0x10B0, SW_PAD_GPIO1_IO03);

    /* 4、设置 GPIO1_IO03 为输出功能 */
    val = readl(GPIO1_GDIR);
    val &= ~(1 << 3); /* 清除以前的设置 */
    val |= (1 << 3); /* 设置为输出 */
    writel(val, GPIO1_GDIR);

    /* 5、默认关闭 LED */
    val = readl(GPIO1_DR);
    val |= (1 << 3);
    writel(val, GPIO1_DR);
}

int get_dt_data(void)
{
    u32 ret = 0;
    const char *str = NULL;
    struct property *proper;
    u32 regdata[14];

    /* 1.获取设备节点：alpha */
    newchrled.nd = of_find_node_by_path("/alphaled");
    if(newchrled.nd == NULL)
    {
        printk("alphaled node cant find\r\n");
        return -EINVAL;
    }
    else
    {
        printk("alphaled node has been found\r\n");
    }

    /* 2.获取compatible属性内容 */
    proper = of_find_property(newchrled.nd, "compatible", NULL);
    if(proper == NULL)
    {
        printk("compatible property find failed\r\n");
        return -EINVAL;
    }
    else
    {
        printk("compatible = %s\r\n", (char *)proper->value);
    }

    /* 3.获取status属性内容 */
    ret = of_property_read_string(newchrled.nd, "status", &str);
    if(ret < 0)
    {
        printk("status read failed\r\n");
    }
    else
    {
        printk("status = %s\r\n", str);
    }

    /* 获取reg属性 */
    ret = of_property_read_u32_array(newchrled.nd, "reg", regdata, 10);
    if(ret < 0)
    {
        printk("reg property read failed\r\n");
    }
    else
    {
        u8 i = 0;
        printk("reg data:\r\n");
        for(i = 0; i < 10; i++)
        {
            printk("%#X ",regdata[i]);
        }
    }

    IMX6U_CCM_CCGR1 = of_iomap(newchrled.nd, 0);
    SW_MUX_GPIO1_IO03 = of_iomap(newchrled.nd, 1);
    SW_PAD_GPIO1_IO03 = of_iomap(newchrled.nd, 2);
    GPIO1_DR = of_iomap(newchrled.nd, 3);
    GPIO1_GDIR = of_iomap(newchrled.nd, 4);


    return 0;
}

static int led_init(void) 
{
 //   u32 setval;
    get_dt_data();
    /* 初始化LED */
    led_hd_init();    

    printk("led_init\n");

    if(newchrled.major)
    {
        newchrled.devid = MKDEV(newchrled.major,0);
        register_chrdev_region(newchrled.devid, NEWCHRDEV_CNT, NEWCHRDEV_NAME);
    }
    else
    {
        alloc_chrdev_region(&newchrled.devid, 0, NEWCHRDEV_CNT, NEWCHRDEV_NAME);   
        newchrled.major = MAJOR(newchrled.devid);
        newchrled.minor = MINOR(newchrled.devid);
    }
    printk("newcheled major=%d,minor=%d\n", newchrled.major, newchrled.minor);

    newchrled.cdev.owner = THIS_MODULE;
    cdev_init(&newchrled.cdev, &newchrled_fops);

    cdev_add(&newchrled.cdev, newchrled.devid, NEWCHRDEV_CNT);

    newchrled.class = class_create(newchrled.cdev.owner, NEWCHRDEV_NAME);
    if(IS_ERR(newchrled.class))
    {
        return PTR_ERR(newchrled.class);
    }    

    newchrled.device = device_create(newchrled.class, NULL, newchrled.devid, NULL, NEWCHRDEV_NAME);
    if(IS_ERR(newchrled.device))
    {
        return PTR_ERR(newchrled.device);
    }    


    return 0;
}


static void led_exit(void) 
{
    printk("led_exit\n");
    iounmap(IMX6U_CCM_CCGR1);
    iounmap(SW_MUX_GPIO1_IO03);
    iounmap(SW_PAD_GPIO1_IO03);
    iounmap(GPIO1_DR);
    iounmap(GPIO1_GDIR);
    
    cdev_del(&newchrled.cdev);
    unregister_chrdev_region(newchrled.devid, NEWCHRDEV_CNT);
    device_destroy(newchrled.class, newchrled.devid);
    class_destroy(newchrled.class);
    // class_unregister(newchrled.class);

    return ;
}

module_init(led_init);
module_exit(led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jacobe Zang");