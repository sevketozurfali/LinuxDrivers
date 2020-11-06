#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

int simple_module_function(void){
    printk(KERN_ALERT "Inside the %s function", __FUNCTION__);
}

EXPORT_SYMBOL(simple_module_function);

int simple_module_init(void){
    printk(KERN_ALERT "Inside the %s function.", __FUNCTION__);
    return 0;
}

void simple_module_exit(void){
    printk(KERN_ALERT "Inside the %s function. ", __FUNCTION__);
}

module_init(simple_module_init);
module_exit(simple_module_exit);