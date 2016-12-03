#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    int inputFd, outputFd;

    if (argc != 3) {
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

    int hole = 0;
    struct stat st;
    char zero[1024];

    if (fstat(inputFd, &st) == -1) {
        errExit("fstat %s", argv[1]);
    }

    if (S_ISREG(st.st_mode) && st.st_size / 512 > st.st_blocks) {
        hole = 1;
        memset(zero, 0, 1024);
    }

    char buf[1024];
    ssize_t count;

    while ((count = read(inputFd, buf, 1024)) > 0) {
        if (hole && !memcmp(buf, zero, count) 
                && lseek(outputFd, count, SEEK_CUR) == -1) {
            errExit("lseek %s", argv[2]);
        } else if (write(outputFd, buf, count) != count) {
            fatal("couldn't write whole buffer");
        }
    }

    if (count == -1) {
        errExit("read %s", argv[1]);
    }

    if (close(inputFd) == -1) {
        errExit("close %s", argv[1]);
    }

    if (close(outputFd) == -1) {
        errExit("close %s", argv[2]);
    }

    exit(EXIT_SUCCESS);
}
