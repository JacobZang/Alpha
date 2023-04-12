#include "asm/uaccess.h"
#include <linux/fs.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/ide.h>
#include <linux/module.h>

#define CHRDEVBASE_MAJOR 200
#define CHRDEVBASE_NAME "chrdevbase"

static char readbuf[100];
static char writebuf[100];
static char kerneldata[] = {"kernel data"};

static int chrdevbase_open(struct inode *inode, struct file *filep)
{
    printk("chrdevbase_open\r\n");
    return 0;
}

static int chrdevbase_read(struct file *filep, char __user *buf, size_t cnt, loff_t *offt)
{
    int retvalue = 0;

    memcpy(readbuf, kerneldata, sizeof(kerneldata));
    retvalue = copy_to_user(buf, readbuf, cnt);
    if(retvalue == 0)
    {
        printk("kernel send data successfully\r\n");
    }
    else
    {
        printk("kernel send data failed\r\n");
    }
    return 0;
}

static int chrdevbase_write(struct file *filep, const char __user *buf, size_t cnt, loff_t *offt)
{
    int retval = 0;
    retval = copy_from_user(writebuf, buf, cnt);
    if(retval == 0)
    {
        printk("usr data: %s\r\n", writebuf);
    }
    else
    {
        printk("kernel receive data failed\r\n");
    }

    return 0;
}
int chrdevbase_release(struct inode *inode, struct file *filep)
{
    printk("chrdevbase_release\r\n");
    return 0;
}

static struct file_operations chrdevbase_fops = 
{
    .owner = THIS_MODULE,
    .open = chrdevbase_open,
    .read = chrdevbase_read,
    .write = chrdevbase_write,
    .release = chrdevbase_release,
};

static int __init chrdevbase_init(void)
{
    int retvalue = 0;

    retvalue = register_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME, &chrdevbase_fops);
    if(retvalue < 0)
    {
        printk("chrdevbase_init failure\r\n");
    }
    else
    {
        printk("chrdevbase_init success\r\n");
    }
    return 0;
}

static void __exit chrdevbase_exit(void)
{
    unregister_chrdev(CHRDEVBASE_MAJOR, CHRDEVBASE_NAME);
    printk("chrdevbase_exit\r\n");    
    return ;
}

module_init(chrdevbase_init);
module_exit(chrdevbase_exit);