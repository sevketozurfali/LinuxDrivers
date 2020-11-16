#include <linux/kernel.h>
// #include <stdio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/interrupt.h>
#include <asm/io.h>



#define mem_size 1024
#define IRQ_NO 1
unsigned int i = 0;

void tasklet_func(unsigned long data);

/*Declare the tasklet */

DECLARE_TASKLET(tasklet,tasklet_func,1);

#define WR_DATA _IOW('a','a',int32_t*)
#define RD_DATA _IOR('a','a',int32_t*)

int32_t val = 0;

static irqreturn_t irq_handler(int irq, void *dev_id){
    printk(KERN_INFO "Keyboard interrupt occured. %d \n", i);
    tasklet_schedule(&tasklet);
    i++;
    return IRQ_HANDLED;

}

volatile int chr_value = 0;

dev_t dev = 0;
static struct class *dev_class;
static struct cdev techo_cdev;
uint8_t *kernel_buffer;

static int __init chr_driver_init(void);
static void __exit chr_driver_exit(void);

static int techo_open (struct inode *tinode, struct file *tfile);
static ssize_t techo_read (struct file *tfile, char __user *buf, size_t len, loff_t *off);
static ssize_t techo_write (struct file *tfile, const char __user *buf, size_t len, loff_t *off);
static int techo_release (struct inode *tinode, struct file *tfile);
static long techo_ioctl(struct file *tfile, unsigned int cmd, unsigned long arg);

static struct file_operations fops = {
    .owner          =   THIS_MODULE,
    .read           =   techo_read,
    .write          =   techo_write,
    .open           =   techo_open,
    .unlocked_ioctl =   techo_ioctl,
    .release        =   techo_release
};


void tasklet_func(unsigned long data){
    printk(KERN_INFO "Tasklet function executed.");
}

static int techo_open(struct inode *tinode, struct file *tfile){
    if((kernel_buffer = kmalloc(mem_size, GFP_KERNEL) ) == 0){
        printk(KERN_INFO "Linux cannot allocate memory for the device....");
        return -1;
    }

    printk(KERN_INFO "Device file opened...");
    return 0;
}

static int techo_release (struct inode *tinode, struct file *tfile){
    kfree(kernel_buffer);
    printk(KERN_INFO "Device driver released sucessfully....");
}

static ssize_t techo_read (struct file *tfile, char __user *buf, size_t len, loff_t *off){
    copy_to_user(buf, kernel_buffer, mem_size);
    printk(KERN_INFO "Data read done..");
    return mem_size;
}


static ssize_t techo_write (struct file *tfile, const char __user *buf, size_t len, loff_t *off){
    copy_from_user(kernel_buffer, buf, len);
    printk(KERN_INFO "Data is written sucessfully.");
    return len;
}


static long techo_ioctl(struct file *tfile, unsigned int cmd, unsigned long arg){
    switch (cmd)
    {
    case WR_DATA:
        copy_from_user(&val, (int32_t*)arg, sizeof(val));
        printk(KERN_INFO "Val : %d \n",val);
        break;
    case RD_DATA:
        copy_to_user((int32_t*)arg, &val, sizeof(val));
        break;
    
    default:
        break;
    }

    return 0;
}

static int __init chr_driver_init(void){
    /* allocating major number*/

    if((alloc_chrdev_region(&dev, 0, 1, "techo-genius")) < 0){
        printk(KERN_INFO "Cannot allocate the major number...!");
        return -1;
    }

    printk(KERN_INFO "Major = %d | Minor = %d", MAJOR(dev), MINOR(dev));

    /* creating cdev structure */

    cdev_init(&techo_cdev, &fops);

    /*adding char device to the system */

    if((cdev_add(&techo_cdev, dev, 1)) < 0){

        printk(KERN_INFO "Can not add device...!");
        goto r_class;
        
    }

    /* creating class */

    if((dev_class = class_create(THIS_MODULE, "techo_class")) == NULL){
        printk(KERN_INFO "Class can not be created...!");
        goto r_class;
    }


    /* creating device */

    if((device_create(dev_class, NULL, dev, NULL, "techo_device")) == NULL ){
        printk(KERN_INFO "Device can not created...!");
        goto r_device;
    }

    if(request_irq(IRQ_NO, irq_handler, IRQF_SHARED, "techo_device", (void *)(irq_handler))){
        printk(KERN_INFO "techo device can not register irq\n");
        goto irq;
    }

    printk(KERN_INFO "Device driver insert.. done properly....");
    return 0;


    irq:
        free_irq(IRQ_NO,(void *)(irq_handler));

    r_device:
        class_destroy(dev_class);


    r_class:
        unregister_chrdev_region(dev, 1);
        return -1;

}

void __exit chr_driver_exit(void){
    free_irq(IRQ_NO,(void *)(irq_handler));
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&techo_cdev);
    unregister_chrdev_region(dev, 1);

    printk(KERN_INFO "Device driver is removed succesfully...");
}


module_init(chr_driver_init);
module_exit(chr_driver_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Şevket Özurfalı");
MODULE_DESCRIPTION("Device driver simple for beginning.");