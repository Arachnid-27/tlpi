#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <sys/epoll.h>

int main(int argc, char *argv[]) {
    int epfd, ready;
    struct epoll_event ev;

    epfd = epoll_create(2);

    ev.events = EPOLLOUT;
    ev.data.fd = STDOUT_FILENO;
    assert(epoll_ctl(epfd, EPOLL_CTL_ADD, STDOUT_FILENO, &ev) != -1);

    ev.events = EPOLLOUT;
    ev.data.fd = STDERR_FILENO;
    assert(epoll_ctl(epfd, EPOLL_CTL_ADD, STDERR_FILENO, &ev) != -1);

    for (int i = 0; i < 10; ++i) {
        ready = epoll_wait(epfd, &ev, 1, -1);
        assert(ready == 1);
        if (i % 2 == 0) {
            assert(ev.data.fd == STDOUT_FILENO);
        } else {
            assert(ev.data.fd == STDERR_FILENO);
        }
    }

    printf("Exercise63-7 succeed!\n");

    exit(EXIT_SUCCESS);
}
