#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");


int ex04_simple_module_function1(void){
	printk(KERN_ALERT "Inside the %s function\n", __FUNCTION__);
	return 0;
}

int ex04_simple_module_function2(void){
	printk(KERN_ALERT "ınside the %s function\n", __FUNCTION__);
	return 0;
}

__initdata int count = 5;
__init int ex01_simple_module_init(void)
{
	int index = 0;
	printk(KERN_ALERT "Inside the %s function\n",__FUNCTION__);
	for(index = 0; index < count; index++){
		printk(KERN_ALERT "Index = %d \n",index);
	}
	
	return 0;
}

void ex01_simple_module_exit(void){
	printk(KERN_ALERT "Inside the %s function\n",__FUNCTION__);
}

module_init(ex01_simple_module_init);
module_exit(ex01_simple_module_exit);


