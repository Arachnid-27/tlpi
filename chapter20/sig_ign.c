#include <signal.h>
#include <assert.h>
#include "tlpi_hdr.h"

int flag;

static void handler(int sig) {
    if (sig == SIGUSR1) {
        flag = 1;
    }
}

int main(int argc, char *argv[]) {
    assert(signal(SIGUSR1, handler) != SIG_ERR);
    assert(flag == 0);
    assert(raise(SIGUSR1) == 0);
    assert(flag == 1);

    flag = 0;

    sigset_t block, pending;

    sigemptyset(&pending);
    sigemptyset(&block);
    sigaddset(&block, SIGUSR1);

    assert(sigismember(&block, SIGUSR1) == 1);
    assert(sigprocmask(SIG_BLOCK, &block, NULL) != -1);
    assert(flag == 0);
    assert(raise(SIGUSR1) == 0);
    assert(sigpending(&pending) == 0);
    assert(sigismember(&pending, SIGUSR1) == 1);
    assert(signal(SIGUSR1, SIG_IGN) != SIG_ERR);
    assert(sigprocmask(SIG_UNBLOCK, &block, NULL) != -1);
    assert(flag == 0);

    assert(signal(SIGUSR1, handler) != SIG_ERR);
    assert(flag == 0);
    assert(raise(SIGUSR1) == 0);
    assert(flag == 1);

    printf("Exercise20-2 succeed!\n");

    exit(EXIT_SUCCESS);
}
