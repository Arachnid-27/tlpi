#include <sys/uio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

int writev2(int fd, const struct iovec *iov, int iovcnt) {
    ssize_t total = 0;

    for (int i = 0; i < iovcnt; ++i) {
        total += iov[i].iov_len;
    }

    char *buf = (char*) malloc(total);

    total = 0;

    for (int i = 0; i < iovcnt; ++i) {
        memmove(buf + total, iov[i].iov_base, iov[i].iov_len);
        total += iov[i].iov_len;
    }

    return write(fd, buf, total);
}

int main() {
    struct iovec iov[3];
    int x = 100;
    float f = 50.0;
    char s[37] = "abcdefghijklmnopqrstuvwxyz1234567890";
    ssize_t total = 0;

    iov[0].iov_base = &x;
    iov[0].iov_len = sizeof(int);
    total += iov[0].iov_len;

    iov[1].iov_base = &f;
    iov[1].iov_len = sizeof(float);
    total += iov[1].iov_len;

    iov[2].iov_base = s;
    iov[2].iov_len = 37 * sizeof(char);
    total += iov[2].iov_len;

    int fd = open("test", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);

    assert(fd != -1);
    assert(writev2(fd, iov, 3) == total);

    printf("Exercise5-7 writev succeed!\n");

    exit(EXIT_SUCCESS);
}
