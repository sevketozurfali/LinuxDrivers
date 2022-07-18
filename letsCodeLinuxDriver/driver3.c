#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>



#define DRIVER_NAME "my_driver"
#define DRIVER_CLASS "my_module_class"

/*  Meta Information */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sevket Ozurfali");
MODULE_DESCRIPTION("This is the second simple major minor number exercise.");

static char buffer[255];
static int buffer_pointer;

dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;


static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs){
    int to_copy, not_copied, delta;

    to_copy = min(count, buffer_pointer);

    not_copied = copy_to_user(user_buffer, buffer, to_copy);

    delta = to_copy - not_copied;

    return delta;
}

static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs){
    int to_copy, not_copied, delta;

    to_copy = min(count, sizeof(buffer));

    not_copied = copy_from_user(buffer, user_buffer, to_copy);
    buffer_pointer = to_copy;

    delta = to_copy - not_copied;

    return delta;
}

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
    .release    =   driver_close,
    .read       =   driver_read,
    .write       =   driver_write
};


static int __init ModuleInit(void){
    printk(KERN_INFO"Hello, Kernel!\n");

    /*allocate device number*/

    if(alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0){
        printk(KERN_INFO"Device nr couldn't be allocated.\n");
        return -1;
    }

    printk(KERN_INFO"Device number creation is succcess. MAJOR : %d, MINOR : %d\n", MAJOR(my_device_nr), 1);

    /*Initialize device file */
    cdev_init(&my_device, &fops);

    /*Registering device to kernel */
    if (cdev_add(&my_device, my_device_nr, 1) < 0){
        printk(KERN_INFO"Registering of device to kernel failed...\n");
        goto addError;
    }

    /* create device class */
    if((my_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL){
        printk(KERN_INFO"Device class can not be created...\n");
        goto classError;
    }

    /* create device file */
    if(device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL){
        printk(KERN_INFO"Cannot create device file...\n");
        goto fileError;
    }

    

    return 0;
addError:
    device_destroy(my_class, my_device_nr);
fileError:
    class_destroy(my_class);
classError:
    unregister_chrdev(my_device_nr, DRIVER_NAME);
    return -1;



}

static void __exit ModuleExit(void){
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    cdev_del(&my_device);
    unregister_chrdev(my_device_nr, DRIVER_NAME);
    printk(KERN_INFO"GoodBye, Kernel!\n");
}


module_init(ModuleInit);
module_exit(ModuleExit);

