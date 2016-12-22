#include <signal.h>
#include <assert.h>
#include "tlpi_hdr.h"

int count;

static void handler(int sig) {
    if (sig == SIGURG) {
        if (count++ == 1) {
            assert(raise(SIGURG) == 0);
            assert(raise(SIGURG) == 0);
            assert(raise(SIGURG) == 0);
        }
    }
}

int main(int argc, char *argv[]) {
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    sa.sa_flags = SA_RESETHAND;

    assert(count == 0);
    assert(sigaction(SIGURG, &action, NULL) != -1);
    for (int i = 0; i < 100000; ++i) {
        assert(raise(SIGURG) == 0);
    }
    assert(count == 1);

    sa.sa_flags = 0;

    assert(count == 1);
    assert(sigaction(SIGURG, &action, NULL) != -1);
    assert(raise(SIGURG) == 0);
    assert(count == 3);

    count = 1;
    sa.sa_flags = SA_NODEFER;

    assert(count == 1);
    assert(sigaction(SIGURG, &action, NULL) != -1);
    assert(raise(SIGURG) == 0);
    assert(count == 5);

    printf("Exercise20-3 succeed!\n");

    exit(EXIT_SUCCESS);
}
