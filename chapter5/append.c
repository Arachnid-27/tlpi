#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        usageErr("%s file\n", argv[0]);
    }

    int fd;

    if ((fd = open(argv[1], O_WRONLY | O_APPEND)) == -1) {
        errExit("open %s", argv[1]);
    }

    if (lseek(fd, 0, SEEK_SET) == -1) {
        errExit("lseek %s", argv[1]);
    }

    const char *buf = "[APPEND]";

    if (write(fd, buf, strlen(buf)) == -1) {
        errExit("write %s", argv[1]);
    }

    exit(EXIT_SUCCESS);
}
