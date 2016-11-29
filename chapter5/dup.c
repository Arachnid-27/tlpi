#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <stdio.h>

int dup(int oldfd) {
    return fcntl(oldfd, F_DUPFD);
}

int dup2(int oldfd, int newfd) {
    if (fcntl(oldfd, F_GETFL) == -1) {
        errno = EBADF;
        return -1;
    }

    if (fcntl(newfd, F_GETFL) != -1) {
        close(newfd);
    }

    if (fcntl(oldfd, F_DUPFD, newfd) == -1) {
        return -1;
    }

    return newfd;
}

int main() {

    int fd = open("test", O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR);
    int fd2 = dup(fd);
    assert(fcntl(fd, F_GETFL) != -1);
    assert(fcntl(fd2, F_GETFL) != -1);
    assert(fcntl(fd, F_GETFL) == fcntl(fd2, F_GETFL));

    int fd3 = 0;
    int fd4 = dup2(fd, fd3);
    assert(fd3 == fd4);
    assert(fcntl(fd, F_GETFL) != -1);
    assert(fcntl(fd4, F_GETFL) != -1);
    assert(fcntl(fd, F_GETFL) == fcntl(fd4, F_GETFL));

    int fd5 = 100;
    int fd6 = dup2(fd, fd5);
    assert(fd5 == fd6);
    assert(fcntl(fd, F_GETFL) != -1);
    assert(fcntl(fd6, F_GETFL) != -1);
    assert(fcntl(fd, F_GETFL) == fcntl(fd6, F_GETFL));

    int fd7 = 200;
    assert(fcntl(fd7, F_GETFL) == -1);
    assert(dup2(fd7, fd) == -1);
    assert(fcntl(fd, F_GETFL) != -1);
    assert(errno == EBADF);

    printf("Exercise5-4 & Exercise5-5 succeed!\n");

    return 0;

}
