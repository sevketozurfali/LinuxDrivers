#include <linux/module.h>
#include <linux/init.h>

/*  Meta Information */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sevket Ozurfali");
MODULE_DESCRIPTION("This is the first simple helloworld exercise.");


static int __init ModuleInit(void){
    printk(KERN_INFO"Hello, Kernel!\n");
    return 0;
}

static void __exit ModuleExit(void){
    printk(KERN_INFO"GoodBye, Kernel!\n");
}


module_init(ModuleInit);
module_exit(ModuleExit);

