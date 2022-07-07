#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>

int real_open(struct inode *pinode, struct file *pfile){
    printk(KERN_ALERT "Inside the %s function", __FUNCTION__);
    return 0;
}

ssize_t real_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset){
    printk(KERN_ALERT "Inside the %s function", __FUNCTION__);
    return 0;
}

ssize_t real_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset){
    printk(KERN_ALERT "Inside the %s function", __FUNCTION__);
    return length;
}

int real_close(struct inode *pinode, struct file *pfile){
    printk(KERN_ALERT "Inside the %s function", __FUNCTION__);
    return 0;
}

struct file_operations real_file_operations = 
{
    .owner = THIS_MODULE,
    .open = real_open,
    .read = real_read,
    .write = real_write,
    .release = real_close,
};

int real_module_init(void){
    printk(KERN_ALERT "Inside the %s function", __FUNCTION__);

    register_chrdev(128, "Real Simple Char Device.",&real_file_operations);
    return 0;
}


void real_module_exit(void){
    printk(KERN_ALERT "Inside the %s function", __FUNCTION__);
    unregister_chrdev(128, "Real Simple Char Device.");
}


module_init(real_module_init);
module_exit(real_module_exit);


