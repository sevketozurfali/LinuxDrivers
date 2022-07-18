#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>


struct i2c_data{
    struct i2c_client *client;
    dev_t dev;  //for major and minor number
    u8 *buf;
    u16 value;
    struct cdev cdev;
    struct class *class;
};

static int my_open(struct inode *inode, struct file *file){
    
    struct i2c_data *dev = container_of(inode->i_cdev, struct i2c_data, cdev);
    if(dev == NULL){
        printk(KERN_ALERT"There is no data...\n");
        return -1;
    }
    file->private_data = dev;
    return 0;
}

static int my_release(struct inode *inode, struct file *file){

    return 0;
}

static ssize_t my_write(struct file *filp, const char *buf, size_t len, loff_t *off){

    struct i2c_data *dev = (struct i2c_data*)(filp->private_data);
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

static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off){
    struct i2c_data *dev = (struct i2c_data*)(filp->private_data);
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


static struct file_operations fops = {
    .read       = my_read,
    .write      = my_write,
    .open       = my_open,
    .release    = my_release,
};

/* Remove function */

static int ads1115_remove (struct i2c_client *client){

    struct i2c_data *data;
    printk(KERN_INFO"Remove Function is invoked");
    data = i2c_get_clientdata(client);
    cdev_del(&data->cdev);
    device_destroy(data->class, data->dev);
    class_destroy(data->class);
    unregister_chrdev_region(data->dev, 1);
    return 0;
}

/* Probe function to invoke the i2c driver */

static int ads1115_probe(struct i2c_client *client, const struct i2c_device_id *id){
    struct i2c_data *data;
    int result;
    printk(KERN_INFO"Probe function is invoked...\n");
    data = devm_kzalloc(&client->dev, sizeof(struct i2c_data), GFP_KERNEL);
    data->value = 30;
    data->buf = devm_kzalloc(&client->dev, data->value, GFP_KERNEL);
    i2c_set_clientdata(client, data);
    result = alloc_chrdev_region(&data->dev, 0, 1, "my_ads_i2c");

    if(result < 0){
        printk(KERN_ALERT"Device Registration");
        unregister_chrdev_region(data->dev, 1);
        return -1;
    }

    printk(KERN_INFO"Major Number = %d\n", MAJOR(data->dev));

    if((data->class = class_create(THIS_MODULE,"i2cdriver")) == NULL){
        printk(KERN_ALERT"Unable to create device class...\n");
        unregister_chrdev_region(data->dev, 1);
        return -1;
    }

    if(device_create(data->class, NULL, data->dev, NULL, "i2c_drv%d", 2) == NULL){
        printk(KERN_ALERT"Unable to create the device...\n");
        class_destroy(data->class);
        unregister_chrdev_region(data->dev, 1);
        return -1;
    }

    cdev_init(&data->cdev, &fops);

    if(cdev_add(&data->cdev, data->dev, 1) == -1){
        printk(KERN_ALERT"Unable to add the device..\n");
        device_destroy(data->class, data->dev);
        class_destroy(data->class);
        unregister_chrdev_region(data->dev, 1);
        return -1;
    }

    return 0;

}

static const struct i2c_device_id i2c_ids[] = {
    {"ads1115", 0},
    {"ads11", 0},
    {}
};

MODULE_DEVICE_TABLE(i2c, i2c_ids);

static struct i2c_driver ads1115_I2C_drv = {
    .driver = {
        .name = "ads11",
        .owner = THIS_MODULE,
    },
    .probe = ads1115_probe,
    .remove = ads1115_remove,
    .id_table = i2c_ids, 
};

/* Init module */

static int __init i2c_client_drv_init(void){

    // Register with i2c-core
    return i2c_add_driver(&ads1115_I2C_drv);
}

/* Exit module */

static void __exit i2c_client_drv_exit(void){
    i2c_del_driver(&ads1115_I2C_drv);
}

module_init(i2c_client_drv_init);
module_exit(i2c_client_drv_exit);

MODULE_DESCRIPTION("I2C Client Driver");
MODULE_AUTHOR("Sevket Ozurfali");
MODULE_LICENSE("GPL");
