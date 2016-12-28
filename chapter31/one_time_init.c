#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

struct once_t once;
int flag;

struct once_t {
    pthread_mutex_t mutex;
    int once;
};

void incr() {
    ++flag;
}

int one_time_init(struct once_t *control, void (*init)(void)) {
    int s;

    if ((s = pthread_mutex_lock(&(control->mutex))) != 0) {
        return s;
    }

    if (!control->once) {
        init();
        control->once = 1;
    }

    if ((s = pthread_mutex_unlock(&(control->mutex))) != 0) {
        return s;
    }

    return 0;
}

int once_init(struct once_t *control) {
    int s;

    if ((s = pthread_mutex_init(&(control->mutex), NULL)) != 0) {
        return s;
    }

    control->once = 0;

    return 0;
}

void *func(void *arg) {
    one_time_init(&once, incr);
    assert(flag == 1);
    return NULL;
}

int main(int argc, char *argv[]) {
    pthread_t tid;

    assert(once_init(&once) == 0);
    assert(pthread_create(&tid, NULL, func, NULL) == 0);
    func(NULL);
    assert(pthread_join(tid, NULL) == 0);
    assert(pthread_create(&tid, NULL, func, NULL) == 0);
    assert(pthread_join(tid, NULL) == 0);
    assert(flag == 1);

    printf("Exercise31-1 succeed!\n");

    exit(EXIT_SUCCESS);
}
