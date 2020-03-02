#include <linux/init.h>
#include <linux/module.h>

void ex03_simple_module_exit(void){
	printk(KERN_ALERT "Inside the %s function\n",__FUNCTION__);
}

module_exit(ex03_simple_module_exit);

