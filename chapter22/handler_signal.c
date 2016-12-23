#include <signal.h>
#include <string.h>
#include <assert.h>
#include "tlpi_hdr.h"

int flag = 0;

static void handler(int sig) {
    /* UNSAFE */

    printf("before handling %d(%s)\n", sig, strsignal(sig));

    sleep(5);

    // kill -40

    printf("after handling %d(%s)\n", sig, strsignal(sig));
}

int main() {
    struct sigaction sa;
    sigset_t ss;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    sa.sa_flags = 0;

    sigfillset(&ss);

    assert(sigaction(SIGUSR1, &sa, NULL) != -1);
    assert(sigaction(SIGUSR2, &sa, NULL) != -1);
    assert(sigaction(40, &sa, NULL) != -1);
    assert(sigprocmask(SIG_BLOCK, &ss, NULL) != -1);

    sleep(15);

    // kill -USR1
    // kill -USR2
    // kill -40

    assert(sigprocmask(SIG_UNBLOCK, &ss, NULL) != -1);

    printf("Exercise22-2 succeed!\n");

    exit(EXIT_SUCCESS);
}
