#include <assert.h>
#include "tlpi_hdr.h"

int main() {
    switch (fork()) {
        case -1:
            errExit("fork");
        case 0:
            sleep(1);
            break;
        default:
            _exit(EXIT_SUCCESS);
    }

    assert(getppid() == 1);

    printf("Exercise26-1 succeed!\n");

    exit(EXIT_SUCCESS);
}
