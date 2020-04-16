obj-m += assignment6.o

all:
	sudo make -C /usr/src/linux-headers-$(shell uname -r) M=$(shell pwd) modules

deadlocks:
	gcc -g user_test_1.c -o user_test_1.o -pthread
	gcc -g user_test_2.c -o user_test_2.o -pthread
	gcc -g user_test_3.c -o user_test_3.o -pthread
	gcc -g user_test_4.c -o user_test_4.o -pthread

cleanup:
	rm -rf *.o *.ko *.mod *.mod.c *.symvers *.order *.cache.mk