#define _POSIX_C_SOURCE 199309
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include "tlpi_hdr.h"

int value;

static void handler(int sig, siginfo_t *siginfo, void *ucontext) {
    if (sig == SIGALRM) {
        value = siginfo->si_value.sival_int;
    }
}

int main() {
    timer_t timerid;
    struct sigaction sa;
    struct itimerspec its;

    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler;
    sa.sa_flags = SA_SIGINFO;

    its.it_interval.tv_sec = 0;
    its.it_interval.tv_nsec = 0;
    its.it_value.tv_sec = 1;
    its.it_value.tv_nsec = 0;

    assert(sigaction(SIGALRM, &sa, NULL) != -1);
    assert(timer_create(ITIMER_REAL, NULL, &timerid) == 0);
    assert(timer_settime(timerid, 0, &its, NULL) == 0);

    pause();

    assert((long) timerid == value);

    printf("Exercise23-3 succeed!\n");

    exit(EXIT_SUCCESS);
}

