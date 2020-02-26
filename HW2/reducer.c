#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>


#define records_length 100


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


void output_record(struct record* rcds) {
    for (int i=0; i<records_length; i++) {
        if (rcds[i].user_id == NULL) { break; }
        printf("(%s,%s,%d)\n", rcds[i].user_id, rcds[i].topic, rcds[i].score);
        rcds[i].score = 0;
        free(rcds[i].user_id);
        free(rcds[i].topic);
        rcds[i].user_id = NULL;
        rcds[i].topic = NULL;
    }
}


static int record_process(struct record* rcds, int offset) {
    // if EOF return if_end = 1
    pthread_mutex_lock(&mtx);

    if (buffers[offset*buffer_size].user_id != NULL) {
        if (strcmp("EOF", buffers[offset*buffer_size].user_id) == 0) {
                output_record(rcds);
                pthread_mutex_unlock(&mtx);
                return 1;
            }
    }

    for (int j=0; j<buffer_size; j++) {
        int rcd_pos = offset*buffer_size+j;
        if (buffers[rcd_pos].user_id == NULL) { break; }

        char user_id[5];
        int score;
        char topic[16];
        user_id[4] = '\0';  // To ensure char array tail is '\0' can be recognized
        topic[15] = '\0';
        strcpy(user_id, buffers[rcd_pos].user_id);
        strcpy(topic, buffers[rcd_pos].topic);
        score = buffers[rcd_pos].score;
        free(buffers[rcd_pos].user_id);
        free(buffers[rcd_pos].topic);
        buffers[rcd_pos].user_id = NULL;
        buffers[rcd_pos].topic = NULL;
        consumed_num++;
    
        // printf("%s,%s,%s\n", user_id, topic, score); //Debug info

        for (int i=0; i<records_length; i++) {
            // init a new record
            if (rcds[i].user_id == NULL) {
                rcds[i].user_id = malloc(sizeof(user_id));
                rcds[i].topic = malloc(sizeof(topic));
                strcpy(rcds[i].user_id, user_id);
                strcpy(rcds[i].topic, topic);
            }

            if (strcmp(topic, rcds[i].topic)==0) {
                rcds[i].score += score;
                break;
            }
        }

        if (consumed_num == produced_num) { pthread_cond_signal(&full); }
    }

    pthread_cond_wait(&empty, &mtx);
    pthread_mutex_unlock(&mtx);
    return 0;
}


static void test(int offset) {
    pthread_mutex_lock(&mtx);
    if (buffers[offset*buffer_size].user_id == NULL) {
        pthread_cond_wait(&empty, &mtx);
    }
    if (!strcmp("TEST", buffers[offset*buffer_size].user_id)) { 
        consumed_num++; 
    }
    printf("tid %d testing: %s\n", offset, buffers[offset*buffer_size].user_id);
    free(buffers[offset*buffer_size].user_id);
    buffers[offset*buffer_size].user_id = NULL;
    pthread_mutex_unlock(&mtx);
}


void* reducer(void* user_table_offset) {
    struct record records[records_length] = {NULL, 0, NULL};
    int if_end = 0;

    while (! if_end) {   
        if_end = record_process(records, (long)user_table_offset);
    }

    // test((long)user_table_offset);   // debug info

    pthread_exit(NULL);
}