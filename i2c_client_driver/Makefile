obj-m := i2c_client_driver.o

export KERNEL_ROOT =$(BASE_INSTALL_DIR)/Step-3/linux-4.15.7
modules:
	@$(MAKE) -C$(KERNEL_ROOT) M=$(shell pwd) modules
clean:
	@$(MAKE) -C$(KERNEL_ROOT) M=$(shell pwd) clean