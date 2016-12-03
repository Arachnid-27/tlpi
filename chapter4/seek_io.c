#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    int fd;

    if (argc < 3 || strcmp(argv[1], "--help") == 0) {
        usageErr("%s file {r<length>|R<length>|w<string>|s<offset>}\n", argv[0]);
    }

    int flags = O_CREAT | O_RDWR;
    mode_t perms = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

    if ((fd = open(argv[1], flags, perms)) == -1) {
        errExit("open");
    }

    ssize_t count, len;
    off_t offset;
    char *buf;

    for (int ap = 2; ap < argc; ++ap) {
        switch(argv[ap][0]) {
            case 'r':
            case 'R':
                len = getLong(&argv[ap][1], GN_ANY_BASE, argv[ap]);

                if ((buf = (char*) malloc(len)) == NULL) {
                    errExit("malloc");
                }

                if ((count = read(fd, buf, len)) == -1) {
                    errExit("read");
                }

                if (count == 0) {
                    printf("%s: end-of-file\n", argv[ap]);
                } else {
                    printf("%s: ", argv[ap]);
                    for (int j = 0; j < count; ++j) {
                        if (argv[ap][0] == 'r') {
                            printf("%c", isprint((unsigned char) buf[j]) ? buf[j] : '?');
                        } else {
                            printf("%02x ", (unsigned) buf[j]);
                        }
                    }
                    printf("\n");
                }

                free(buf);
                break;

            case 'w':
                if ((count = write(fd, &argv[ap][1], strlen(&argv[ap][1]))) == -1) {
                    errExit("write");
                }

                printf("%s: wrote %ld bytes\n", argv[ap], (long) count);
                break;

            case 's':
                offset = getLong(&argv[ap][1], GN_ANY_BASE, argv[ap]);
                if (lseek(fd, offset, SEEK_SET) == -1) {
                    errExit("lseek");
                }

                printf("%s: seek succeeded\n", argv[ap]);
                break;

            default:
                cmdLineErr("Argument must start with [rRws]: %s\n", argv[ap]);
        }
    }

    exit(EXIT_SUCCESS);
}
