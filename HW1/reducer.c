#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


#define input_length 24
#define records_length 100


struct record {
    char*   user_id;
    int     score;
    char*   topic;
};


int char_2_int(char* num) {
    if (num[0] == '-') { return -(num[1]-'0')*10 + (num[2]-'0'); }
    else { return (num[0]-'0')*10 + (num[1]-'0'); }
}


void output_record(struct record* rcd) {
    for (int i=0; i<records_length; i++) {
        if (rcd[i].user_id == NULL) { break; }
        printf("(%s,%s,%d)\n", rcd[i].user_id, rcd[i].topic, rcd[i].score);
        rcd[i].score = 0;
        free(rcd[i].user_id);
        free(rcd[i].topic);
        rcd[i].user_id = NULL;
        rcd[i].topic = NULL;
    }
}


void record_process(struct record* rcd, char* r_input, char* active_id) {
    r_input = strchr(r_input, '(') + 1;
    char* r_input_end = strchr(r_input, ')');
    int score_len = r_input_end - (r_input+21);

    // divide input record into user-id, action and topic
    char user_id[4];
    char score[3] = {};
    char topic[15];
    user_id[4] = '\0';  // To ensure char array tail is '\0' can be recognized
    topic[15] = '\0';
    strncpy(user_id, r_input, 4);
    strncpy(topic, r_input+5, 15);
    strncpy(score, r_input+21, score_len);
    // printf("%s,%s,%s\n", user_id, topic, score); //Debug info

    for (int i=0; i<records_length; i++) {
        // Once active id is different, flush all existed records
        if (strcmp(active_id, user_id)!=0) { output_record(rcd); }
        strcpy(active_id, user_id);

        if (rcd[i].user_id == NULL) {   // init a new record
            rcd[i].user_id = malloc(sizeof(user_id));
            rcd[i].topic = malloc(sizeof(topic));
            strcpy(rcd[i].user_id, user_id);
            strcpy(rcd[i].topic, topic);
        }

        if (strcmp(user_id, rcd[i].user_id)==0 && strcmp(topic, rcd[i].topic)==0) {
            rcd[i].score += char_2_int(score);
            break;
        }
    }
}


int main(int argc, char** argv) {
    char buff[input_length+2+2];   // two buckets & a null-terminator & a "\n" 
    char active_id[4];
    struct record records[records_length] = {NULL, 0, NULL};

    while (1) {   
        // printf("Input a record: ");
        if (fgets(buff, input_length+2+2, stdin) == NULL) { 
            output_record(records);
            printf("END OF STDIN!\n");
            break; 
        }
        // printf("%s", buff);  // Debug info
        if (strchr(buff, '(') == NULL || strchr(buff, ')') == NULL) {
            printf("FORMAT ERROR:");
            printf("%d", strchr(buff, '(') == NULL);    // Debug info
            printf("%d,", strchr(buff, ')') == NULL);  // Debug info
            printf("%s", buff);     // Debug info
            printf("\n"); //Debug info
            break;
        }

        record_process(records, buff, active_id);
    }

    return 0;
}