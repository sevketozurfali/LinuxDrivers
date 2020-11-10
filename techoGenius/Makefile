obj-m := simple_driver.o
obj-m += proc_device_driver.o

KDIR := /lib/modules/4.15.0-122-generic/build/
PWD := $(shell pwd)

default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
install:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) module_install 
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean