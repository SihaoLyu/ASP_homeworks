#include <pthread.h>

// struct record {
//     char*   user_id;
//     int     score;
//     char*   topic;
// };


struct record* buffers = NULL;
char** user_table = NULL;
int buffer_size;
int buffer_num;
int produced_num;
int consumed_num;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

// static struct record* buffers = NULL;
// static char** user_table = NULL;
// static int buffer_size;
// static int buffer_num;
// static int produced_num;
// static int consumed_num;
// static pthread_cond_t full = PTHREAD_COND_INITIALIZER;
// static pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
// static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;