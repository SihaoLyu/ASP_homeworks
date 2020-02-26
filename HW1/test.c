#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void modify(char* a) {
    char b[4];
    char b1[4];
    // printf("%d, %d, \n", b[4], b1[4]);

    memcpy(b, a, 4);
    memcpy(b1, a, 4);

    printf("%s\n", b);
    printf("%s\n", b1);
}

int main(int argc, char** argv) {
    
    
    int i = 5, j = 2;
    printf("%d %d", i << j, i >> j);

    return 0;
}