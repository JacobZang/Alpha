#include "asm-generic/errno-base.h"
#include "asm-generic/int-ll64.h"
#include "asm/io.h"
#include "asm/uaccess.h"
#include "linux/cdev.h"
#include "linux/device.h"
#include "linux/err.h"
#include "linux/export.h"
#include "linux/kdev_t.h"
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
#define NEWCHRDEV_NAME "led" 

#define LEDON  0
#define LEDOFF 1

#define CCM_CCGR1_BASE          (0X020C406C)
#define SW_MUX_GPIO1_IO03_BASE  (0X020E0068)
#define SW_PAD_GPIO1_IO03_BASE  (0X020E02F4)
#define GPIO1_DR_BASE           (0X0209C000)
#define GPIO1_GDIR_BASE         (0X0209C004)

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
    IMX6U_CCM_CCGR1 = ioremap(CCM_CCGR1_BASE, 4);
    SW_MUX_GPIO1_IO03 = ioremap(SW_MUX_GPIO1_IO03_BASE, 4);
    SW_PAD_GPIO1_IO03 = ioremap(SW_PAD_GPIO1_IO03_BASE, 4);
    GPIO1_DR = ioremap(GPIO1_DR_BASE, 4);
    GPIO1_GDIR = ioremap(GPIO1_GDIR_BASE, 4);

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

static int led_init(void) 
{
 //   u32 setval;
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