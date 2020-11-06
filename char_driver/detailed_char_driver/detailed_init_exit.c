#include <linux/init.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>


MODULE_LICENSE("GPL");
static dev_t first;

/* file operations */


int detailed_open (struct inode *pinode, struct file *pfile){
    printk(KERN_ALERT "Inside the %s function.", __FUNCTION__);
    return 0;

}
ssize_t detailed_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset){
    printk(KERN_ALERT "Inside the %s function.", __FUNCTION__);
    return 0;

}
ssize_t detailed_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset){
    printk(KERN_ALERT "Inside the %s function. : %ld", __FUNCTION__, length);
    return length;

}
int detailed_close (struct inode *pinode, struct file *pfile){
    printk(KERN_ALERT "Inside the %s function.", __FUNCTION__);
    return 0;

}



struct file_operations detailed_file_operations = {

    .owner = THIS_MODULE,
    .open = detailed_open,
    .read = detailed_read,
    .write = detailed_write,
    .release = detailed_close,

};

struct class *cl;
static struct cdev c_dev;


int detailed_module_init(void){
    printk(KERN_ALERT "Inside the %s function.", __FUNCTION__);
    

    int ret;
    struct device *dev_ret;
    if((ret = alloc_chrdev_region(&first, 0, 3, "detailed init exit")) < 0){
        return ret;
    }

    //register_chrdev(350, "detailed init exit", &detailed_file_operations);
    // return 0;

    printk(KERN_INFO "<major , minor>: <%d, %d>\n", MAJOR(first), MINOR(first));

    if(IS_ERR(cl = class_create(THIS_MODULE, "chardev"))){
        unregister_chrdev_region(first, 1);
        return PTR_ERR(cl);
    }

    if(IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "detailed_init_exit"))){
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return PTR_ERR(dev_ret);
    }

    cdev_init(&c_dev, &detailed_file_operations);

    if((ret = cdev_add(&c_dev, first, 1)) < 0){
        device_destroy(cl,first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return ret;
    }


    return 0;
}

void detailed_module_exit(void){
    printk(KERN_ALERT "Inside the %s function. ", __FUNCTION__);
    //unregister_chrdev(350, "detailed init exit");
    //unregister_chrdev_region(first, 3);
    //device_destroy(cl, first);
    //class_destroy(cl);

    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_ALERT "detailed_init_exit unregistered.");
}

module_init(detailed_module_init);
module_exit(detailed_module_exit);