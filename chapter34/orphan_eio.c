#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

int main(int argc, char *argv[]) {
    char buf[1024];
    int child;

    switch (child = fork()) {
        case -1:
            exit(EXIT_FAILURE);
        case 0:
            sleep(1);
            assert(getppid() == 1);
            assert(read(STDIN_FILENO, buf, 1024) == -1);
            assert(errno == EIO);
            break;
        default:
            _exit(EXIT_SUCCESS);
    }

    printf("Exercise34-6 succeed!\n");

    exit(EXIT_SUCCESS);
}
