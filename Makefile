ifneq ($(KERNELRELEASE),)

obj-m := demo.o
demo-objs := demo_core.o demo_dev.o demo_interface.o demo_proc.o demo_sysfs.o

obj-m += xxx_demo_driver.o

obj-m += xxx_demo_device.o

else
	
KDIR := /home/apple/raspberry/build/linux-rpi-4.1.y
all:prepare
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-bcm2708-linux-gnueabi-
	cp *.ko ./release/	
prepare:
	cp /home/apple/win_share/driver_module_test/* ./
modules_install:
	make -C $(KDIR) M=$(PWD) modules_install ARCH=arm CROSS_COMPILE=arm-bcm2708-linux-gnueabi-
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers  modul*
	rm -f ./release/*

endif
