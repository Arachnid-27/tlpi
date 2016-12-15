#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usageErr("%s file\n", argv[0]);
    }

    struct stat buf;

    if (stat(argv[1], &buf) == -1) {
        errExit("stat %s", argv[1]);
    }

    mode_t mode = buf.st_mode | S_IRUSR | S_IRGRP | S_IROTH;

    if (S_ISDIR(mode) || (mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
        mode |= (S_IXUSR | S_IXGRP | S_IXOTH);
    }

    if (chmod(argv[1], mode) == -1) {
        errExit("chmod %s", argv[1]);
    }

    exit(EXIT_SUCCESS);
}
