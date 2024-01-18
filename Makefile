CC = gcc
CFLAGS = -std=gnu99 -Wall

obj-m += kfetch_mod_312551132.o
PWD := $(CURDIR)

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

load:
	sudo insmod ./kfetch_mod_312551132.ko
	sudo mknod /dev/kfetch c 509 0

unload:
	sudo rmmod kfetch_mod_312551132
	sudo rm /dev/kfetch

