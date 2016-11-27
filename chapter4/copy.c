#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE
#define BUF_SIZE 1024
#endif

int main(int argc, char *argv[]) {

    int inputFd, outputFd;

    if (argc != 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s old-file new-file\n", argv[0]);
    }

    if ((inputFd = open(argv[1], O_RDONLY)) == -1) {
        errExit("opening file %s", argv[1]);
    }

    int flags = O_CREAT | O_WRONLY | O_TRUNC;
    mode_t perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    if ((outputFd = open(argv[2], flags, perms)) == -1) {
        errExit("opening file %s", argv[2]);
    }

    char buf[BUF_SIZE];
    ssize_t count;

    while ((count = read(inputFd, buf, BUF_SIZE)) > 0) {
        if (write(outputFd, buf, count) != count) {
            fatal("couldn't write whole buffer");
        }
    }

    if (count == -1) {
        errExit("read");
    }

    if (close(inputFd) == -1) {
        errExit("close %s", argv[1]);
    }

    if (close(outputFd) == -1) {
        errExit("close %s", argv[2]);
    }

    exit(EXIT_SUCCESS);

}

