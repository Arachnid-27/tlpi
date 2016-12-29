#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

int main(int argc, char *argv[]) {
    int child;

    switch (child = fork()) {
        case -1:
            exit(EXIT_FAILURE);
        case 0:
            sleep(1);
            assert(getpgrp() == getpid());
            execlp("which", "", (char *) NULL);
            sleep(2);
            _exit(EXIT_SUCCESS);
        default:
            assert(setpgid(child, child) == 0);
            sleep(2);
            assert(setpgid(child, 0) == -1);
            assert(errno == EACCES);
    }

    printf("Exercise34-2 succeed!\n");

    exit(EXIT_SUCCESS);
}
