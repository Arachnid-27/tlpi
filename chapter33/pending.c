#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <signal.h>
#include <pthread.h>

void *func(void *arg) {
    sigset_t *ss = (sigset_t *) malloc(sizeof(sigset_t));

    sleep(1);

    assert(sigpending(ss) == 0);

    return ss;
}

int main(int arg, char *argv[]) {
    sigset_t ss;
    pthread_t t1, t2;
    void *r1, *r2;

    assert(sigfillset(&ss) == 0);
    assert(sigprocmask(SIG_BLOCK, &ss, NULL) != -1);
    assert(pthread_create(&t1, NULL, func, NULL) == 0);
    assert(pthread_create(&t2, NULL, func, NULL) == 0);
    assert(pthread_kill(t1, SIGUSR1) == 0);
    assert(pthread_kill(t2, SIGUSR2) == 0);
    assert(pthread_join(t1, &r1) == 0);
    assert(pthread_join(t2, &r2) == 0);
    assert(sigismember((sigset_t *) r1, SIGUSR1) == 1);
    assert(sigismember((sigset_t *) r1, SIGUSR2) == 0);
    assert(sigismember((sigset_t *) r2, SIGUSR1) == 0);
    assert(sigismember((sigset_t *) r2, SIGUSR2) == 1);

    printf("Exercise33-1 succeed!\n");

    exit(EXIT_SUCCESS);
}
