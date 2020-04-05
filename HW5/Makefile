
obj-m := char_driver.o

KERNEL_DIR = /usr/src/linux-headers-$(shell uname -r)

all:
	$(MAKE) -C $(KERNEL_DIR) SUBDIRS=$(shell pwd)
	
app0: 
	gcc -o userapp userapp.c

app1:
	gcc -o test test.c -pthread

clean:
	rm -rf *.o *.ko *.mod.* *.symvers *.order *~
