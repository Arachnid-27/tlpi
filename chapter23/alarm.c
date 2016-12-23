#include <signal.h>
#include <sys/time.h>
#include <assert.h>
#include "tlpi_hdr.h"

int flag;

unsigned int alarm2(unsigned int seconds) {
    struct itimerval new_it;
    struct itimerval old_it;

    new_it.it_interval.tv_sec = 0;
    new_it.it_interval.tv_usec = 0;
    new_it.it_value.tv_sec = seconds;
    new_it.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, &new_it, &old_it);

    return old_it.it_value.tv_sec;
}

static void handler(int sig) {
    if (sig == SIGALRM) {
        flag = 1;
    }
}

int main() {
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = handler;
    sa.sa_flags = 0;

    assert(sigaction(SIGALRM, &sa, NULL) != -1);
    assert(alarm2(5) == 0);

    sleep(2);

    assert(alarm2(1) == 2);

    pause();

    assert(flag == 1);

    printf("Exercise23-1 succeed!\n");

    exit(EXIT_SUCCESS);
}
