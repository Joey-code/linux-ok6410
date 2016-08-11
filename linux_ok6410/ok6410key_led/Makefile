ifneq ($(KERNELRELEASE),)
obj-m := key_modules.o
else
KDIR := /home/yjj/linux/linux-3.0.1
all:
	make -C $(KDIR) M=$(PWD) modules ARCH=arm CROSS_COMPILE=arm-linux-
clean:
	rm -f *.ko *.o *.mod.o *.mod.c *.symvers  modul*
endif
