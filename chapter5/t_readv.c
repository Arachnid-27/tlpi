#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char* argv[]) {

    if (argc != 2) {
        usageErr("%s file\n", argv[0]);
    }

    int fd;

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        errExit("open %s", argv[1]);
    }

    struct iovec iov[3];
    struct stat st;     // first buffer
    int x;              // second buffer
    char str[1000];     // thrid buffer
    ssize_t total = 0;

    iov[0].iov_base = &st;
    iov[0].iov_len = sizeof(struct stat);
    total += iov[0].iov_len;

    iov[1].iov_base = &x;
    iov[1].iov_len = sizeof(int);
    total += iov[1].iov_len;

    iov[2].iov_base = str;
    iov[2].iov_len = 1000;
    total += iov[2].iov_len;

    ssize_t count;

    if ((count = readv(fd, iov, 3)) == -1) {
        errExit("readv");
    }
    
    if (count < total) {
        printf("Read fewer bytes than requested\n");
    }

    printf("total bytes requested: %ld; bytes read: %ld\n", (long) total, (long) count);

    exit(EXIT_SUCCESS);

}
