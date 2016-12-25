#include <sys/wait.h>
#include <signal.h>
#include <assert.h>
#include "tlpi_hdr.h"

int flag;

static void handler(int sig) {
    flag = 1;
}

int main(int argc, char *argv[]) {
    sigset_t blockMask;
    struct sigaction sa;

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &blockMask, NULL) == -1) {
        errExit("sigprocmask");
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        errExit("sigaction");
    }

    switch (fork()) {
    case -1:
        errExit("fork");
    case 0:
        _exit(EXIT_SUCCESS);
    }

    if (waitpid(-1, NULL, 0) == -1) {
        errExit("waitpid");
    }

    assert(flag == 0);

    if (sigprocmask(SIG_UNBLOCK, &blockMask, NULL) == -1) {
        errExit("sigprocmask");
    }

    assert(flag == 1);

    printf("Exercise27-6 succeed!\n");

    exit(EXIT_SUCCESS);
}
