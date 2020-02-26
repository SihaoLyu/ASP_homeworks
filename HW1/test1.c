#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

void errExit(char* err) {
    printf("%s", err);
    _Exit(-1);
}

int main(int argc, char** argv[]) {
    // int pid = fork();
    // switch(pid) {
    //     case -1:
    //         errExit("fork\n");

    //     case 0:
    //         execl("./mapper", "mapper", "testfile", (char*)NULL);
    //         errExit("ls\n");

    //     default: break;
    // }

    // wait(NULL);
    // printf("%d\n", pid);
    // return 0;

    // printf("Input a record: ");
    // char buff[26];
    // fgets(buff, 26, stdin);
    // printf("%s\n", buff);
    // fgets(buff, 26, stdin);
    // printf("%s\n", buff);

    char* a = "98";
    printf("%d\n", a[0]);

    return 0;
}
