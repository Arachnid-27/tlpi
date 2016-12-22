#include <signal.h>
#include <stdio.h>

void abort2() {
    raise(SIGABRT);

    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = SIG_DFL;
    sa.sa_flags = 0;
    sigaction(SIGABRT, &sa, NULL);

    fflush(NULL);
    fclose(NULL);

    raise(SIGABRT);
}
