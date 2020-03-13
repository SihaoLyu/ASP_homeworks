#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <pthread.h>
#include <iostream>
#include <string>
#include "record.h"
#include "mapper.cpp"
#include "reducer.cpp"


using namespace std;
// #define INT_MIN INT32_MIN
// #define INT_MAX INT32_MAX


// #ifndef struct record
//     struct record {
//         int    user_id = INT_MIN;
//         int    score;
//         char   topic[16];
//     };
// #endif 


struct record* buffers;
int buffer_size;
int buffer_num;
int* produced_num;
int* consumed_num;
pthread_cond_t* full;
pthread_cond_t* empty;
pthread_mutex_t* mtx;


int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: taken buffer size and buffer nums as arguments!\n");
        return 0;
    }

    // Initialize public variables
    buffer_size = atoi(argv[1]);
    buffer_num = atoi(argv[2]);
    buffers = (struct record*)mmap(NULL, sizeof(struct record)*buffer_num*buffer_size, \
                PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    memset(buffers, 0x00, sizeof(*buffers));
    for (int i=0; i<buffer_size*buffer_num; i++) {
        buffers[i].user_id = INT_MIN;
    }
    
    produced_num = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, \
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    consumed_num = (int*)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, \
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    memset(produced_num, 0x00, sizeof(int));
    memset(consumed_num, 0x00, sizeof(int));
    (*produced_num) = 0, (*consumed_num) = 0;

    pthread_condattr_t cond_attr;
    pthread_mutexattr_t mtx_attr;
    memset(&cond_attr, 0x00, sizeof(cond_attr));
    memset(&mtx_attr, 0x00, sizeof(mtx_attr));
    pthread_condattr_init(&cond_attr);
    pthread_mutexattr_init(&mtx_attr);
    pthread_condattr_setpshared(&cond_attr, PTHREAD_PROCESS_SHARED);
    pthread_mutexattr_setpshared(&mtx_attr, PTHREAD_PROCESS_SHARED);

    full = (pthread_cond_t*)mmap(NULL, sizeof(*full), PROT_READ | PROT_WRITE, \
            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    empty = (pthread_cond_t*)mmap(NULL, sizeof(*empty), PROT_READ | PROT_WRITE, \
            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    mtx = (pthread_mutex_t*)mmap(NULL, sizeof(*mtx), PROT_READ | PROT_WRITE, \
            MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    memset(full, 0x00, sizeof(*full));
    memset(empty, 0x00, sizeof(empty));
    memset(mtx, 0x00, sizeof(mtx));
    pthread_cond_init(full, &cond_attr);
    pthread_cond_init(empty, &cond_attr);
    pthread_mutex_init(mtx, &mtx_attr);
    
    // create processes for mapper & reducers
    switch (fork()) {
        case -1: {
            cout << "mapper fork() fault!" << endl;
            return -1;
        }

        case 0: {
            mapper();
            // pthread_condattr_destroy(&cond_attr);
            // pthread_mutexattr_destroy(&mtx_attr);
            return 0;
        }
    }

    for (int i=0; i<buffer_num; i++) {
        switch (fork()) {
            case -1: {
                cout << "reducer fork() fault!" << endl;
                return -1;
            }

            case 0: {
                reducer(i);
                // pthread_condattr_destroy(&cond_attr);
                // pthread_mutexattr_destroy(&mtx_attr);
                return 0;
            }

            default: {
                continue;
            }
        }
    }
    

    // Wait child processes to be done
    for (int i=0; i<buffer_num+1; i++) {
        if (wait(NULL) == -1) {
            cout << "Waiting error!" << endl;
            return -1;
        }
    }
    pthread_condattr_destroy(&cond_attr);
    pthread_mutexattr_destroy(&mtx_attr);
    pthread_cond_destroy(full);
    pthread_cond_destroy(empty);
    pthread_mutex_destroy(mtx);
    munmap(buffers, sizeof(struct record)*buffer_num*buffer_size);
    munmap(full, sizeof(*full));
    munmap(empty, sizeof(*empty));
    munmap(mtx, sizeof(*mtx));
    munmap(produced_num, sizeof(int));
    munmap(consumed_num, sizeof(int));

    return 0;
}