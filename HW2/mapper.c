#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>


#define input_length 22


static struct record {
    char*   user_id;
    int     score;
    char*   topic;
};


extern struct record* buffers;
extern char** user_table;
extern int buffer_size;
extern int buffer_num;
extern int produced_num;
extern int consumed_num;
extern pthread_cond_t full;
extern pthread_cond_t empty;
extern pthread_mutex_t mtx;


int action_2_weight(char action) {
    switch (action) {
        case 'P':   return 50;
        case 'L':   return 20;
        case 'D':   return -10;
        case 'C':   return 30;
        case 'S':   return 40;
        default:    return 0;   // ACTION NOT EXISTS!
    }
}


int find_buffer_pos(char* user_id) {
    for (int i=0; i<buffer_num; i++) {
        if (user_table[i] == NULL) {
            user_table[i] = malloc(sizeof(user_id));
            strcpy(user_table[i], user_id);
            return i;
        }
        if (strcmp(user_id, user_table[i]) == 0) { return i; }
    }
    return -1;
}


static void record_process(FILE* f) {
    pthread_mutex_lock(&mtx);
    // check if any buffer full
    for (int i=0; i<buffer_num; i++) {
        if (buffers[i*buffer_size+buffer_size-1].user_id != NULL) {
            pthread_cond_broadcast(&empty);
            pthread_cond_wait(&full, &mtx);
            break;
        }
    }

    char r_input[input_length+1];   // plus a null-terminator
    fgets(r_input, input_length+1, f);    //input a record

    // divide input record into user-id, action and topic
    char user_id[5];
    char action;
    char topic[16];
    int score;
    user_id[4] = '\0';
    topic[15] = '\0';
    memcpy(user_id, r_input, 4);
    memcpy(topic, r_input+7, 15);
    action = r_input[5];
    score = action_2_weight(action);

    int user_pos = find_buffer_pos(user_id);
    if (user_pos == -1) { 
        printf("# user numbers are not sufficient!\n"); exit(0);
    }
    for (int i=0; i<buffer_size; i++) {
        int rcd_pos = user_pos*buffer_size+i;
        if (buffers[rcd_pos].user_id == NULL) {
            buffers[rcd_pos].user_id = malloc(sizeof(user_id));
            buffers[rcd_pos].topic = malloc(sizeof(topic));
            buffers[rcd_pos].score = score;
            strcpy(buffers[rcd_pos].user_id, user_id);
            strcpy(buffers[rcd_pos].topic, topic);
            produced_num++;
            break;
        }
    }
    
    pthread_mutex_unlock(&mtx);
}


void notify_end() {
    // let reducers consume remaining buffers
    pthread_mutex_lock(&mtx);
    for (int i=0; i<buffer_num; i++) {
        if (buffers[i*buffer_size].user_id != NULL) {
            pthread_cond_broadcast(&empty);
            pthread_cond_wait(&full, &mtx);
        }
    }

    // mark the end of input
    for (int i=0; i<buffer_num; i++) {
        buffers[i*buffer_size].user_id = malloc(sizeof(char)*4);
        buffers[i*buffer_size].user_id[4] = '\0';
        strcpy(buffers[i*buffer_size].user_id, "EOF");
    }

    pthread_cond_broadcast(&empty);
    pthread_mutex_unlock(&mtx);
}


static void test() {
    pthread_mutex_lock(&mtx);
    for (int i=0; i<buffer_num; i++) {
        buffers[i*buffer_size].user_id = malloc(sizeof(char)*4);
        buffers[i*buffer_size].user_id[4] = '\0';
        strcpy(buffers[i*buffer_size].user_id, "TEST");
        produced_num++;
    }
    printf("P testing\n");

    pthread_cond_broadcast(&empty);
    pthread_mutex_unlock(&mtx);
}


void* mapper(void* no_use_arg) {
    int temp_c;
    while (! feof(stdin)) {
        temp_c = fgetc(stdin);
        if (temp_c == '(')  { record_process(stdin); }    //input a record
    }

    notify_end();

    // test();  // debug info

    pthread_exit(NULL);
}