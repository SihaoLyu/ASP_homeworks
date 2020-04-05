#include <linux/ioctl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <pthread.h>

#define DEVICE "/dev/mycdrv"

#define CDRV_IOC_MAGIC 'Z'
#define ASP_CLEAR_BUF _IOW(CDRV_IOC_MAGIC, 1, int)

void* t_1(void* arg) {
    int fd = open("/dev/mycdrv0", O_RDWR);
    if(fd == -1) {
		printf("File %s either does not exist or has been locked by another "
				"process\n", DEVICE);
		return;
	}
    ioctl(fd, ASP_CLEAR_BUF, 0);

    char buf[10];
    write(fd, "2\0", 2);
    while (strcmp(buf, "-2")) { 
        printf("%s\n", buf);
        lseek(fd, 0, SEEK_SET);
        read(fd, buf, sizeof(buf)); 
    }
    ioctl(fd, ASP_CLEAR_BUF, 0);
    write(fd, "1\0", 2);
    close(fd);
    return;
}

void* t_2(void* arg) {
    int fd = open("/dev/mycdrv0", O_RDWR);
    if(fd == -1) {
		printf("File %s either does not exist or has been locked by another "
				"process\n", DEVICE);
		return;
	}

    char buf[10];
    char id[10];
    sprintf(id, "%lu", (long)arg);
    while (strcmp(buf, id)) { 
        lseek(fd, 0, SEEK_SET);
        read(fd, buf, sizeof(buf)); 
        printf("%s\n", buf);
    }
    lseek(fd, 0, SEEK_SET);
    sprintf(id, "-%lu", (long)arg);
    write(fd, id, sizeof(id));
    close(fd);
    return;
}

int main(int argc, char *argv[]) {
    pthread_t tid[3];
    pthread_create(&tid[0], NULL, t_1, NULL);
    pthread_create(&tid[1], NULL, t_2, (void*)1);
    pthread_create(&tid[2], NULL, t_2, (void*)2);

    pthread_join(tid[0], NULL);
    pthread_join(tid[1], NULL);
    pthread_join(tid[2], NULL);

    return 0;
}