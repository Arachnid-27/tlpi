#include <assert.h>
#include "tlpi_hdr.h"

int main() {
    switch (fork()) {
        case -1:
            errExit("fork");
        case 0:
            switch (fork()) {
                case -1:
                    errExit("fork");
                case 0:
                    sleep(1);
                    assert(getppid() == 1);
                    printf("Exercise26-2 succeed!\n");
                default:
                    _exit(EXIT_SUCCESS);
            }
            break;
        default:
            sleep(3);
    }

    exit(EXIT_SUCCESS);
}
