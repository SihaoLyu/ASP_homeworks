#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>


void errExit(char* err) {
    printf("%s", err);
    _Exit(-1);
}


int main(int argc, char** argv) {
    if (argc != 2) { 
        printf("Usage: only take 1 argument which should be file name!\n"); return -1; 
    }
    char *testfile_name = argv[1];
    int fd[2];
    if (pipe(fd) == -1) { errExit("pipe\n"); }

    switch (fork()) {
        case -1: 
            errExit("fork mapper\n");

        case 0:
            if (close(fd[0]) == -1) { errExit("close_err mapper: read_end\n"); }

            if (fd[1] != STDOUT_FILENO) {
                if (dup2(fd[1], STDOUT_FILENO) == -1) { errExit("dup2 mapper\n"); }
                if (close(fd[1]) == -1) { errExit("close_err mapper: write_end\n"); }
            }

            execl("./mapper", "mapper", testfile_name, (char*)NULL);
            // execlp("ls", "ls", (char*)NULL);
            errExit("open mapper error\n");

        default:
            break;
    }

    switch (fork()) {
        case -1: 
            errExit("fork reducer\n");

        case 0:
            if (close(fd[1]) == -1) { errExit("close_err reducer: write_end\n"); }

            if (fd[0] != STDIN_FILENO) {
                if (dup2(fd[0], STDIN_FILENO) == -1) { errExit("dup2 reducer\n"); }
                if (close(fd[0]) == -1) { errExit("close_err reducer: write_end\n"); }
            }

            execl("./reducer", "reducer", (char*)NULL);
            // execlp("wc", "wc", "-l", (char*)NULL);
            errExit("open reducer error\n");

        default:
            break;
    }

    if (close(fd[0]) == -1) { errExit("PProcess close read end err\n"); }
    if (close(fd[1]) == -1) { errExit("PProcess close write end err\n"); }
    if (wait(NULL) == -1) { errExit("no zombie child process\n"); }
    if (wait(NULL) == -1) { errExit("no zombie child process\n"); }

    return 0;
}