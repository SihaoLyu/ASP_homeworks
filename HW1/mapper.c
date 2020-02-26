#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define input_length 22
#define records_length 100


struct record {
    char*   user_id;
    int     score;
    char*   topic;
};


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


void record_process(struct record* rcd, FILE* f) {
    char r_input[input_length+1];   // plus a null-terminator
    fgets(r_input, input_length+1, f);    //input a record

    // divide input record into user-id, action and topic
    char user_id[4];
    char action;
    char topic[15];
    user_id[4] = '\0';
    topic[15] = '\0';
    memcpy(user_id, r_input, 4);
    action = r_input[5];
    memcpy(topic, r_input+7, 15);

    for (int i=0; i<records_length; i++) {
        if (rcd[i].user_id == NULL) {   // init a new record
            rcd[i].user_id = (char*)malloc(sizeof(user_id));
            rcd[i].topic = (char*)malloc(sizeof(topic));
            strcpy(rcd[i].user_id, user_id);
            strcpy(rcd[i].topic, topic);
            rcd[i].score = action_2_weight(action);
            break;
        }

        // if (strcmp(user_id, rcd[i].user_id)==0 && strcmp(topic, rcd[i].topic)==0) {
        //     rcd[i].score += action_2_weight(action);
        //     break;
        // }
    }
}


void output_record(struct record* rcd) {
    for (int i=0; i<records_length; i++) {
        if (rcd[i].user_id == NULL) { break; }
        printf("(%s,%s,%d)\n", rcd[i].user_id, rcd[i].topic, rcd[i].score);
        // if (i<records_length-1 && rcd[i+1].user_id != NULL) { printf(",\n"); }
        // if (i==records_length-1 || rcd[i+1].user_id == NULL) { printf("\n"); }
        rcd[i].score = 0;
        free(rcd[i].user_id);
        free(rcd[i].topic);
        rcd[i].user_id = NULL;
        rcd[i].topic = NULL;
    }
}


int main(int argc, char** argv) {
    FILE* f;
    if (argc != 2) { printf("Usage: only take 1 argument which should be file path!\n"); return -1; }
    if (! (f = fopen(argv[1], "r"))) { printf("FILE PATH WRONG!\n"); return -1;}

    int temp_c;
    struct record records[records_length] = {NULL, 0, NULL};
    while (! feof(f)) {
        temp_c = fgetc(f);
        if (temp_c == '(')  { record_process(records, f); }
    }

    output_record(records);
    fclose(f);
    return 0;
}