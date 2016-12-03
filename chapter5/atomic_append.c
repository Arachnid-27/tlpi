#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        usageErr("%s filename num-bytes [x]", argv[0]);
    }

    int flags = O_CREAT | O_WRONLY;
    mode_t perms = S_IRUSR | S_IWUSR;

    if (argc == 3) {
        flags |= O_APPEND;
    }

    int fd;

    if ((fd = open(argv[1], flags, perms)) == -1) {
        errExit("open %s", argv[1]);
    }

    const char *buf = "x";
    
    for (int count = getInt(argv[2], GN_ANY_BASE, NULL); count > 0; --count) {
        if (argc != 3) {
            if (lseek(fd, 0, SEEK_END) == -1) {
                errExit("lseek %s", argv[1]);
            }
        }
        if (write(fd, buf, 1) == -1) {
            errExit("write %s", argv[1]);
        }
    }

    exit(EXIT_SUCCESS);
}
