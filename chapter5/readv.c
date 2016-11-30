#include <sys/uio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdio.h>

int readv2(int fd, const struct iovec *iov, int iovcnt) {

    ssize_t total = 0;

    for (int i = 0; i < iovcnt; ++i) {
        total += iov[i].iov_len;
    }

    char *buf = (char*) malloc(total);

    if (read(fd, buf, total) == -1) {
        return -1;
    }

    total = 0;

    for (int i = 0; i < iovcnt; ++i) {
        memmove(iov[i].iov_base, buf + total, iov[i].iov_len);
        total += iov[i].iov_len;
    }

    return total;

}

int main() {

    struct iovec iov[3];
    int x;
    float f;
    char s[37];
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

    int fd = open("test", O_RDONLY);

    assert(fd != -1);
    assert(readv2(fd, iov, 3) == total);
    assert(x == 100);
    assert(f == 50.0);
    assert(!strcmp(s, "abcdefghijklmnopqrstuvwxyz1234567890"));

    printf("Exercise5-7 readv succeed!\n");

    return 0;

}
