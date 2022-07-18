#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/device.h>
#include <asm/uaccess.h>


#define mem_size 1024

dev_t dev = 0;

// static struct class *dev_class;
// static struct cdev my_cdev;
uint8_t *kernel_buffer;

static int __init base_driver_init(void);
static void __exit base_driver_exit(void);

struct i2c_data{
    struct i2c_client *client;
    dev_t dev;
    u8 *buf;
    u16 value;
    struct class *dev_class;
    struct cdev my_cdev;
};



static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *file, const char *buf, size_t len, loff_t *off);


static struct file_operations fops = {
    .owner      =   THIS_MODULE,
    .read       =   my_read,
    .write      =   my_write,
    .open       =   my_open,
    .release    =   my_release,
};

static int my_open(struct inode *inode, struct file *file){
    // /*Creating physical memory */
    // if((kernel_buffer = kmalloc(mem_size, GFP_KERNEL)) == 0 ){
    //     printk(KERN_INFO"Cannot allocate the memory to the kernel...\n");
    //     return -1;
    // }
    // printk(KERN_INFO"Device file opened...\n");
    // return 0;

    struct i2c_data *dev = container_of(inode->i_cdev, struct i2c_data, my_cdev);
    if(dev == NULL){
        printk(KERN_ALERT"There is no data...\n");
        return -1;
    }
    file->private_data = dev;
    return 0;

}

static int my_release(struct inode *inode, struct file *file){
    // kfree(kernel_buffer);
    // printk(KERN_INFO"Device file closed...\n");
    // return 0;

    return 0;
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *off){
    // copy_to_user(buf, kernel_buffer, mem_size);
    // printk(KERN_INFO"Data read : DONE\n");
    // return mem_size;

    struct i2c_data *dev = (struct i2c_data*)(file->private_data);
    struct i2c_adapter *adap = dev->client->adapter;
    struct i2c_msg msg;
    char *temp;
    int ret ;

    temp = kmalloc(len, GFP_KERNEL);

    msg.addr = 0x23;
    msg.flags = 0;
    msg.flags |= I2C_M_RD;
    msg.len = len;
    msg.buf = temp;

    ret = i2c_transfer(adap, &msg, 1);
    if(ret >= 0){
        ret = copy_to_user(buf, temp, len) ? -EFAULT : len;
    }
    kfree(temp);
    
    return (ret == 1? len :ret) ;
}

static ssize_t my_write(struct file *file, const char *buf, size_t len, loff_t *off){
    // copy_from_user(kernel_buffer, buf, len);
    // printk(KERN_INFO"Data is written successfully....\n");
    // return len;

    struct i2c_data *dev = (struct i2c_data*)(file->private_data);
    struct i2c_adapter *adap = dev->client->adapter;
    struct i2c_msg msg;
    char *temp;
    int ret ;

    temp = memdup_user(buf, len);

    msg.addr = 0x23;
    msg.flags = 0;
    msg.len = len;
    msg.buf = temp;

    ret = i2c_transfer(adap, &msg, 1);
    kfree(temp);
    
    return (ret == 1? len :ret) ;
}

// static int __init base_driver_init(void){
//     /* Allocating Major number */
//     if((alloc_chrdev_region(&dev, 0, 1, "my_dev")) < 0){
//         printk(KERN_INFO"Cannot allocate the major number...\n");
//         return -1;
//     }

//     printk(KERN_INFO"Major = %d Minor = %d...\n", MAJOR(dev), MINOR(dev));

//     /* creating cdev structure */
//         cdev_init(&my_cdev,&fops);

//     /* Adding characther device to the system...*/
//         if((cdev_add(&my_cdev, dev, 1)) < 0){
//             printk(KERN_INFO"Cannot add the device to the system...\n");
//             goto r_class;
//         }

//     /* Creating struct class */
//         if((dev_class = class_create(THIS_MODULE, "my_class")) == NULL){
//             printk(KERN_INFO"Cannot create the struct class...\n");
//             goto r_class;
//         }

//     /* createing device*/
//         if((device_create(dev_class, NULL, dev, NULL, "my_device")) == NULL){
//             printk(KERN_INFO"Cannot create the device...\n");
//             goto r_device;
//         }
//         printk(KERN_INFO"Device driver inserted properly.\n");
//         return 0;

// r_device:
//     class_destroy(dev_class);



// r_class:
//     unregister_chrdev_region(dev, 1);
//     return -1;


// }

// probe function to invoke the i2c driver.

static int bh1750_probe(struct i2c_client *client, const struct i2c_device_id *id){
    
    struct i2c_data *data;
    int result;

    printk(KERN_INFO"Probe func is invoked...\n");
    data = devm_kzalloc(&client->dev, sizeof(struct i2c_data), GFP_KERNEL);
    data->value = 30;
    data->buf = devm_kzalloc(&client->dev, data->value,GFP_KERNEL);
    i2c_set_clientdata(client, data);

    result = alloc_chrdev_region(&data->dev, 0, 1, "bh_drv");

    if(result < 0){
        printk(KERN_ALERT"Unable to Device Registration...\n");
        unregister_chrdev_region(data->dev, 1);
        return -1;
    }

    printk(KERN_INFO"Major Number : %d Minor number : %d\n", MAJOR(data->dev), MINOR(data->dev));

    if((data->dev_class = class_create(THIS_MODULE,"i2cdriver")) == NULL){
        printk(KERN_ALERT"Unable to create device class...\n");
        unregister_chrdev_region(data->dev, 1);
        return -1;
    }

    if(device_create(data->dev_class, NULL, data->dev, NULL, "i2c_drv%d", 2) == NULL){
        printk(KERN_ALERT"Unable to create the device...\n");
        class_destroy(data->dev_class);
        unregister_chrdev_region(data->dev, 1);
        return -1;
    }

    cdev_init(&data->my_cdev, &fops);

    if(cdev_add(&data->my_cdev, data->dev, 1) == -1){
        printk(KERN_ALERT"Unable to add the device..\n");
        device_destroy(data->dev_class, data->dev);
        class_destroy(data->dev_class);
        unregister_chrdev_region(data->dev, 1);
        return -1;
    }

    return 0;

}

/* Remove function */
static int bh1750_remove (struct i2c_client *client){
    
    struct i2c_data *data;
    printk(KERN_INFO"Remove Function is invoked");
    data = i2c_get_clientdata(client);
    cdev_del(&data->my_cdev);
    device_destroy(data->dev_class, data->dev);
    class_destroy(data->dev_class);
    unregister_chrdev_region(data->dev, 1);
    
    return 0;
}

static const struct i2c_device_id i2c_ids[] = {
    {"bh1750", 0},
    {"bh17", 0},
    { },
};

MODULE_DEVICE_TABLE(i2c, i2c_ids);

static struct i2c_driver bh1750_I2C_drv = {
    .driver = {
        .name = "bh17",
        .owner = THIS_MODULE,
    },
    .probe = bh1750_probe,
    .remove = bh1750_remove,
    .id_table = i2c_ids,
};

static int __init base_driver_init(void){

    return i2c_add_driver(&bh1750_I2C_drv);

}

void __exit base_driver_exit(void){
    // device_destroy(dev_class,dev);
    // class_destroy(dev_class);
    // cdev_del(&my_cdev);
    // unregister_chrdev_region(dev, 1);
    // printk(KERN_INFO"Device driver is removed successfully...\n");
    i2c_del_driver(&bh1750_I2C_drv);
}



module_init(base_driver_init);
module_exit(base_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sevket ozurfali sevketozurfali@gmail.com");
MODULE_DESCRIPTION("This is the base driver...");