KDIR = /home/tero/raspberry/linux-rpi-3.18.y

export CROSS_COMPILE = /home/tero/raspberry/raspberry_tools/arm-bcm2708/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/bin/arm-linux-gnueabihf-

export ARCH = arm

ifneq ($(KERNELRELEASE),)
	obj-m := rpigpio.o

else

default:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

endif

