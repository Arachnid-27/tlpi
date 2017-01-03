#include <ctype.h>
#include "tlpi_hdr.h"

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    int count;
    int pfd1[2], pfd2[2];
    char buf[BUF_SIZE];

    if (pipe(pfd1) == -1 || pipe(pfd2) == -1) {
        errExit("pipe");
    }

    switch (fork()) {
    case -1:
        errExit("fork");
    case 0:
        if (close(pfd1[1]) == -1 || close(pfd2[0]) == -1) {
            errExit("close");
        }

        while ((count = read(pfd1[0], buf, BUF_SIZE)) > 0) {
            for (int i = 0; i < count; ++i) {
                buf[i] = toupper(buf[i]);
            }

            if (write(pfd2[1], buf, count) != count) {
                errExit("write");
            }
        }

        if (close(pfd1[0]) == -1 || close(pfd2[1]) == -1) {
            errExit("close");
        }
        
        _exit(EXIT_SUCCESS);
    default:
        if (close(pfd1[0]) == -1 || close(pfd2[1]) == -1) {
            errExit("close");
        }

        while ((count = read(STDIN_FILENO, buf, BUF_SIZE)) > 0) {
            if (write(pfd1[1], buf, count) != count) {
                errExit("write");
            }

            if ((count = read(pfd2[0], buf, BUF_SIZE)) > 0) {
                if (write(STDOUT_FILENO, buf, count) != count) {
                    errExit("write");
                }
            }
        }

        if (close(pfd1[1]) == -1 || close(pfd2[0]) == -1) {
            errExit("close");
        }
    }

    exit(EXIT_SUCCESS);
}
