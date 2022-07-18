#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

#define MYMAJOR 91
#define MYMINOR 1

/*  Meta Information */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sevket Ozurfali");
MODULE_DESCRIPTION("This is the second simple major minor number exercise.");

static int driver_open(struct inode *device_file, struct file *instance){
    printk("Dev_nr - open was called.\n");
    return 0;
}

static int driver_close(struct inode *device_file, struct file *instance){
    printk("Dev_nr - close was called.\n");
    return 0;
}

static struct file_operations fops = {
    .owner      =   THIS_MODULE,
    .open       =   driver_open,
    .release    =   driver_close
};


static int __init ModuleInit(void){
    int retval;

    printk(KERN_INFO"Hello, Kernel!\n");
    /*Register device*/
    retval = register_chrdev(MYMAJOR, "my_driver", &fops);
    if(retval == 0){
        printk(KERN_INFO"Dev number registered device number : Major %d, Minor %d\n", MYMAJOR, 1);
    }
    else if(retval > 0){
        printk(KERN_INFO"Dev number registered device number : Major %d, Minor %d\n", retval>>20, retval&0xfffff);
    }
    else{
        printk(KERN_INFO"Could not register device number\n");
        return -1;
    }
    return 0;
}

static void __exit ModuleExit(void){
    unregister_chrdev(MYMAJOR, "my_driver");
    printk(KERN_INFO"GoodBye, Kernel!\n");
}


module_init(ModuleInit);
module_exit(ModuleExit);

