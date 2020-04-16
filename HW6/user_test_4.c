#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/wait.h>

#define TID_NUM 5
#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)

void* t_op(void* arg) {
    sleep(3);
    int fd;
    fd = open("/dev/a5", O_RDWR);
    ioctl(fd, E2_IOCMODE1);
    close(fd);
    printf("Done!\n");
}

int main(int argc, char** argv) {
    pthread_t tid;
    pthread_create(&tid, NULL, t_op, NULL);

    int fd;
    fd = open("/dev/a5", O_RDWR);
    ioctl(fd, E2_IOCMODE2);
    pthread_join(tid, NULL);
    close(fd);

    return 0;
}