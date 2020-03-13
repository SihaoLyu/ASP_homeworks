#ifndef record_h
#define record_h
#include <stdint.h>

#define INT_MIN INT32_MIN
#define INT_MAX INT32_MAX

struct record {
    int     user_id;
    int     score;
    char    topic[16];
};
#endif