#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define TID_NUM 2
#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)

void* t_op_0(void* arg) {
    int fd;
    char dev_path[] = "/dev/a5";
    fd = open(dev_path, O_RDWR);
    printf("%d\n", fd);

    ioctl(fd, E2_IOCMODE2);
    sleep(2);
    ioctl(fd, E2_IOCMODE1);
    close(fd);
}

void* t_op_1(void* arg) {
    sleep(1);
    int fd;
    char dev_path[] = "/dev/a5";
    fd = open(dev_path, O_RDWR);
    printf("%d\n", fd);

    ioctl(fd, E2_IOCMODE1);
    close(fd);
}

int main(int argc, char** argv) {
    pthread_t tid[TID_NUM];
    pthread_create(&tid[0], NULL, t_op_0, NULL);
    pthread_create(&tid[1], NULL, t_op_1, NULL);


    for (int i=0; i<TID_NUM; i++) {
        pthread_join(tid[i], NULL);
    }
    return 0;
}