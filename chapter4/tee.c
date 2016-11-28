#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s [-a] file\n", argv[0]);
    }

    int fd;
    int flags = O_CREAT | O_WRONLY | O_TRUNC;
    mode_t perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    while (getopt(argc, argv, "a") != -1) {
        flags = O_CREAT | O_WRONLY | O_APPEND;
    }

    if ((fd = open(argv[optind], flags, perms)) == -1) {
        errExit("open file %s", argv[optind]);
    }

    char buf[1024];
    ssize_t count;

    while ((count = read(STDIN_FILENO, buf, 1024)) > 0) {
        if (write(STDOUT_FILENO, buf, count) != count || write(fd, buf, count) != count) {
            fatal("couldn't write whole buffer");
        }
    }

    if (count == -1) {
        errExit("read stdin");
    }

    if (close(fd) == -1) {
        errExit("close %s", argv[optind]);
    }
 
    exit(EXIT_SUCCESS);

}
