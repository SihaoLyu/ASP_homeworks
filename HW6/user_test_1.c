#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define TID_NUM 5
#define CDRV_IOC_MAGIC 'Z'
#define E2_IOCMODE1 _IOWR(CDRV_IOC_MAGIC, 1, int)
#define E2_IOCMODE2 _IOWR(CDRV_IOC_MAGIC, 2, int)

void* t_op(void* arg) {
    int fd;
    char dev_path[] = "/dev/a5";
    fd = open(dev_path, O_RDWR);
    printf("%d\n", fd);

    sleep(1);
    ioctl(fd, E2_IOCMODE2);
    close(fd);
}

int main(int argc, char** argv) {
    pthread_t tid[TID_NUM];
    for (int i=0; i<TID_NUM; i++) {
        pthread_create(&tid[i], NULL, t_op, NULL);
    }

    // sleep(1000);

    for (int i=0; i<TID_NUM; i++) {
        pthread_join(tid[i], NULL);
    }
    return 0;
}