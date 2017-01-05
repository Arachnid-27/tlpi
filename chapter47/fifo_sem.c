#include <sys/stat.h>
#include <assert.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#define FIFO_SEM "/tmp/fifo_sem"

int init() {
    int fd;
    umask(0);
    if (mkfifo(FIFO_SEM, S_IRUSR | S_IWUSR) == -1 && errno != EEXIST) {
        return -1;
    }
    if ((fd = open(FIFO_SEM, O_RDWR)) != -1) {
        unlink(FIFO_SEM);
        return fd;
    }
    return -1;
}

int release(int fd) {
    const char *buf = "x";
    if (write(fd, &buf, 1) == 1) {
        return 0;
    }
    return 1;
}

int reserve(int fd) {
    char buf[1];
    if (read(fd, &buf, 1) == 1) {
        return 0;
    }
    return -1;
}

int reserve_no_block(int fd) {
    char buf[1];
    int flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
    int count = read(fd, &buf, 1);
    if (count == 1) {
        flags &= ~O_NONBLOCK;
        fcntl(fd, F_SETFL, flags);
        return 0;
    }
    flags &= ~O_NONBLOCK;
    fcntl(fd, F_SETFL, flags);
    return -1;
}

int main(int argc, char *argv[]) {
    int fd = init();

    switch (fork()) {
    case -1:
        errExit("fork");
    case 0:
        assert(reserve(fd) == 0);
        assert(reserve_no_block(fd) == -1);
        assert(errno == EAGAIN);

        printf("Exercise47-6 succeed!\n");

        _exit(EXIT_SUCCESS);
    default:
        sleep(2);
        assert(release(fd) == 0);
    }

    exit(EXIT_SUCCESS);
}
