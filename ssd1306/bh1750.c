#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define I2C_BUS_AVAILABLE   (       2)
#define SLAVE_DEVICE_NAME   ("bh1750")
#define BH1750_SLAVE_ADDR   (    0x23)
#define DRIVER_CLASS "my_module_class"
#define mem_size 64

static struct i2c_adapter *bh_i2c_adapter       = NULL;
static struct i2c_client *bh_i2c_client         = NULL;

int32_t value = 0;

uint8_t *kernel_buffer;

dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;


static long int I2C_Read(const struct i2c_client * current_client, u8 out_buf, unsigned int len){
    
    long s32 = i2c_smbus_read_word_data ( current_client, out_buf);

    return s32;
}

static int BH1750_data(u8 data){
    int ret;

    ret = I2C_Read(bh_i2c_client, data, 2);

    int lightPerc = ((ret * 100) / 65279);

    return lightPerc;
}

static int bh_probe(struct i2c_client *client, const struct i2c_device_id *id){

    printk("BH Probed!!!!\n");

    return 0;

}

static int bh_remove(struct i2c_client *client){
    printk("BH Removed!!!\n");
    return 0;
}

static const struct i2c_device_id bh_id[] = {
    {SLAVE_DEVICE_NAME, 0},
    {}
};

MODULE_DEVICE_TABLE(i2c, bh_id);

static struct i2c_driver bh_driver = {
    .driver = {
        .name = SLAVE_DEVICE_NAME,
        .owner = THIS_MODULE,
    },
    .probe      = bh_probe,
    .remove     = bh_remove,
    .id_table   = bh_id,
};



static struct i2c_board_info bh_i2c_board_info = 
{
    I2C_BOARD_INFO(SLAVE_DEVICE_NAME, BH1750_SLAVE_ADDR)
};

static long driver_ioctl(struct file *file, unsigned int cmd, unsigned long arg){
    
    value = BH1750_data(0x20);

    if(copy_to_user((int32_t*)arg, &value, sizeof(value))){
        printk("Data write : Err\n");
    }
    return 0;
}

static ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs){

    printk(KERN_INFO"Read value detected...\n");

    return 0;
}

static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs){

    return 0;
}

static int driver_open(struct inode *device_file, struct file *instance){

    return 0;
}

static int driver_close(struct inode *device_file, struct file *instance){

    return 0;
}

static struct file_operations fops = {
    .owner      =   THIS_MODULE,
    .open       =   driver_open,
    .release    =   driver_close,
    .read       =   driver_read,
    .write       =   driver_write,
    .unlocked_ioctl= driver_ioctl
};

static int __init bh_driver_init(void){
    int ret = -1;

    if(alloc_chrdev_region(&my_device_nr, 0, 1, SLAVE_DEVICE_NAME) < 0){
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
    if(device_create(my_class, NULL, my_device_nr, NULL, SLAVE_DEVICE_NAME) == NULL){
        printk(KERN_INFO"Cannot create device file...\n");
        goto fileError;
    }

        bh_i2c_adapter = i2c_get_adapter(I2C_BUS_AVAILABLE);

    if(bh_i2c_adapter != NULL){
        bh_i2c_client = i2c_new_client_device(bh_i2c_adapter, &bh_i2c_board_info);

        if(bh_i2c_client != NULL){
            i2c_add_driver(&bh_driver);
            ret = 0;
        }

        i2c_put_adapter(bh_i2c_adapter);
    }
    printk("Driver Added !!!! \n");
    return ret;

    

    return ret;
addError:
    device_destroy(my_class, my_device_nr);
fileError:
    class_destroy(my_class);
classError:
    unregister_chrdev(my_device_nr, SLAVE_DEVICE_NAME);
    return -1;


}

static void __exit bh_driver_exit(void){
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    cdev_del(&my_device);
    i2c_unregister_device(bh_i2c_client);
    i2c_del_driver(&bh_driver);
    printk("Driver Removed...\n");
}


module_init(bh_driver_init);
module_exit(bh_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sevket Ozurfali sevketozurfali@gmail.com");
MODULE_DESCRIPTION("Sample I2c driver of BH1750 module.");
MODULE_VERSION("0.1");