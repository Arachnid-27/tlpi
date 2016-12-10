#include <unistd.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usageErr("%s [-n num] file\n", argv[0]);
    }
    
    int num = 10;

    if (getopt(argc, argv, "n:") != -1) {
        num = getInt(optarg, GN_ANY_BASE, optarg);
    }

    int fd;

    if ((fd = open(argv[optind], O_RDONLY)) == -1) {
        errExit("open %s", argv[optind]);
    }

    char buf[4096];
    int lines = 0;
    off_t total;

    if ((total = lseek(fd, -1, SEEK_END)) == -1) {
        errExit("lseek %s", argv[optind]);
    }

    while (lines <= num && total > 0) {
        int count = total > 4096 ? 4096 : total;

        if (lseek(fd, -count, SEEK_CUR) == -1) {
            errExit("lseek %s", argv[optind]);
        }

        if (read(fd, buf, count) != count) {
            errExit("read %s", argv[optind]);
        }

        if (lseek(fd, -count, SEEK_CUR) == -1) {
            errExit("lseek %s", argv[optind]);
        }

        for (int i = count - 1; i >= 0; --i) {
            if (buf[i] == '\n') {
                if (lines == num) {
                    if (lseek(fd, i + 1, SEEK_CUR) == -1) {
                        errExit("lseek %s", argv[optind]);
                    }
                    break;
                }
                ++lines;
            }
        }

        total -= count;
    }

    int count;

    while ((count = read(fd, buf, 4096)) > 0) {
        if (write(STDOUT_FILENO, buf, count) != count) {
            errExit("write");
        }
    }

    exit(EXIT_SUCCESS);
}
