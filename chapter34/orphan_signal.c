#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

int flag;

static void handler(int sig) {
    ++flag;
}

int main(int argc, char *argv[]) {
    struct sigaction sa;
    int child;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    sa.sa_flags = 0;

    assert(sigaction(SIGTTIN, &sa, NULL) != -1);
    assert(signal(SIGTSTP, SIG_DFL) != SIG_ERR);

    switch (child = fork()) {
        case -1:
            exit(EXIT_FAILURE);
        case 0:
            sleep(1);
            assert(getppid() == 1);
            assert(raise(SIGTSTP) == 0);
            assert(flag == 0);
            assert(raise(SIGTTIN) == 0);
            assert(flag == 1);
            break;
        default:
            _exit(EXIT_SUCCESS);
    }

    printf("Exercise34-7 succeed!\n");

    exit(EXIT_SUCCESS);
}
