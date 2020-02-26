#include "public_variables.h"

struct record* buffers = NULL;
char** user_table = NULL;
int buffer_size;
int buffer_num;
int produced_num;
int consumed_num;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;