#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>


static struct record {
    char*   user_id;
    int     score;
    char*   topic;
};


struct record* buffers = NULL;
char** user_table = NULL;
int buffer_size;
int buffer_num;
int produced_num;
int consumed_num;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
extern void* mapper(void*);
extern void* reducer(void*);


int char_2_int(char* num) {
    if (num[0] == '-') { return -1; }
    else { 
        int i_num = 0;
        int len = strlen(num);
        for (int i=0; i<len; i++) { 
            i_num += ((int)pow((float)10, (float)(len-i-1))*(num[i]-'0')); 
        }
        return i_num; 
    }
}


int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: taken buffer size and buffer nums as arguments!\n");
        return 0;
    }

    // Initialize public variables
    buffer_size = char_2_int(argv[1]);
    buffer_num = char_2_int(argv[2]);
    produced_num = 0;
    consumed_num = 0;

    buffers = malloc(sizeof(struct record)*buffer_num*buffer_size);
    user_table = malloc(sizeof(char*)*buffer_num);
    for (int i=0; i<buffer_size*buffer_num; i++) {
        buffers[i].user_id = NULL;
        buffers[i].topic = NULL;
        buffers[i].score = 0;
    }
    for (int i=0; i<buffer_num; i++) { user_table[i] = NULL; }

    // create threads for mapper & reducers
    pthread_t tid[buffer_num+1];
    pthread_create(&tid[0], NULL, mapper, NULL);
    for (long i=0; i<buffer_num; i++) {
        pthread_create(&tid[1+i], NULL, reducer, (void*)i);
    }

    for (int i=0; i<buffer_num+1; i++) {
        pthread_join(tid[i], NULL);
    }

    // printf("p: %d, c: %d\n", produced_num, consumed_num);    // debug info

    free(buffers);
    free(user_table);
    buffers = NULL;
    user_table = NULL;

    return 0;
}