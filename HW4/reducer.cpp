#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "record.h"


using namespace std;
#define RCDS_LEN 100
// #define INT_MIN INT32_MIN
// #define INT_MAX INT32_MAX


// #ifndef struct record
//     struct record {
//         int    user_id = INT_MIN;
//         int    score;
//         char   topic[16];
//     };
// #endif 


extern struct record* buffers;
extern int buffer_size;
extern int buffer_num;
extern int* produced_num;
extern int* consumed_num;
extern pthread_cond_t* full;
extern pthread_cond_t* empty;
extern pthread_mutex_t* mtx;


void output_record(struct record* rcds) {
    for (int i=0; i<RCDS_LEN; i++) {
        if (rcds[i].user_id == INT_MIN) { break; }
        printf("(%d,%s,%d)\n", rcds[i].user_id, rcds[i].topic, rcds[i].score);
        rcds[i].score = 0;
    }
}


static int record_process(struct record* rcds, int offset) {
    // if EOF return if_end = 1
    pthread_mutex_lock(mtx);

    if (buffers[offset*buffer_size].user_id == INT_MAX) {
        output_record(rcds);
        pthread_mutex_unlock(mtx);
        return 1;
    }

    for (int j=0; j<buffer_size; j++) {
        int rcd_pos = offset*buffer_size+j;
        if (buffers[rcd_pos].user_id == INT_MIN) { break; }

        int user_id;
        int score;
        char topic[16];
        topic[15] = '\0';
        user_id = buffers[rcd_pos].user_id;
        strcpy(topic, buffers[rcd_pos].topic);
        score = buffers[rcd_pos].score;
        buffers[rcd_pos].user_id = INT_MIN;
        (*consumed_num)++;
    
        // printf("%s,%s,%s\n", user_id, topic, score); //Debug info

        for (int i=0; i<RCDS_LEN; i++) {
            // init a new record
            if (rcds[i].user_id == INT_MIN) {
                rcds[i].user_id = user_id;
                strcpy(rcds[i].topic, topic);
            }

            if (strcmp(topic, rcds[i].topic)==0) {
                rcds[i].score += score;
                break;
            }
        }

        if (*consumed_num == *produced_num) { pthread_cond_signal(full); }
    }

    pthread_cond_wait(empty, mtx);
    pthread_mutex_unlock(mtx);
    return 0;
}


static void test(int offset) {
    pthread_mutex_lock(mtx);
    if (buffers[offset*buffer_size].user_id == INT_MIN) {
        cout << "111: " << offset << endl;
        pthread_cond_wait(empty, mtx);
        cout << "222: " << offset << endl;
    }
    if (buffers[offset*buffer_size].user_id == INT_MAX) { 
        (*consumed_num)++; 
        // cout << *consumed_num << endl;
    }
    printf("tid %d testing: %d\n", offset, buffers[offset*buffer_size].user_id);
    pthread_mutex_unlock(mtx);
}


int reducer(int user_table_offset) {
    struct record records[RCDS_LEN];
    for (int i=0; i<RCD_SIZE; i++) {
        records[i].user_id = INT_MIN;
        records[i].score = 0;
        strcpy(records[i].topic, "");
    }
    int if_end = 0;

    while (! if_end) {   
        if_end = record_process(records, user_table_offset);
    }

    // test(user_table_offset);   // debug info

    return 0;
}