#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

int count = 1;

module_param(count, int, 0);

int simple_module_param_init(void){
    printk(KERN_ALERT "Inside the %s function", __FUNCTION__);
    printk(KERN_ALERT "");

    int index = 0;

    for(index = 0; index < count; index++){
        printk(KERN_ALERT "Hello sevket : index = %d\n", index);
    }


    return 0;
}

void simple_module_param_exit(void){
    printk(KERN_ALERT "Inside the %s function", __FUNCTION__);
    printk(KERN_ALERT "");
}

module_init(simple_module_param_init);
module_exit(simple_module_param_exit);

