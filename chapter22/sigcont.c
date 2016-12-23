#include <signal.h>
#include <assert.h>
#include "tlpi_hdr.h"

int flag = 0;

static void handler(int sig) {
    if (sig == SIGCONT) {
        flag = 1;
    }
}

int main() {
    struct sigaction sa;
    sigset_t ss;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    sa.sa_flags = 0;

    sigemptyset(&ss);
    sigaddset(&ss, SIGCONT);

    assert(sigaction(SIGCONT, &sa, NULL) != -1);
    assert(sigprocmask(SIG_BLOCK, &ss, NULL) != -1);

    sleep(5);

    // Ctrl-Z
    // kill -CONT

    assert(flag == 0);
    assert(sigprocmask(SIG_UNBLOCK, &ss, NULL) != -1);
    assert(flag == 1);
    
    printf("Exercise22-1 succeed!\n");

    exit(EXIT_SUCCESS);
}
