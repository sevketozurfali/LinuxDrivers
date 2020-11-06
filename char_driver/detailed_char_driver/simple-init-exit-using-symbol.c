#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

int simple_module_function(void);

int simple_module_init(void){
    printk(KERN_ALERT "Inside the %s function.", __FUNCTION__);
    simple_module_function();
    return 0;
}

void simple_module_exit(void){
    printk(KERN_ALERT "Inside the %s function. ", __FUNCTION__);
}

module_init(simple_module_init);
module_exit(simple_module_exit);