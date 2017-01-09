#include <sys/mman.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>
#include "tlpi_hdr.h"

int flag;
char *addr;

void handler(int sig) {
    if (sig == SIGBUS) {
        assert(flag == 0);
        flag = 1;
        char c = addr[8192];
    } else if (sig == SIGSEGV) {
        assert(flag == 1);
        printf("Exercise49-4 succeed!\n");
        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[]) {
    int fd;
    char temp[] = "/tmp/tempXXXXXX";
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;

    if (sigaction(SIGBUS, &sa, NULL) == -1) {
        errExit("sigaction SIGBUS");
    }

    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        errExit("sigaction SIGSEGV");
    }

    if ((fd = mkstemp(temp)) == -1) {
        errExit("kmstemp");
    }

    if (ftruncate(fd, 2048) == -1) {
        errExit("ftruncate");
    }

    unlink(temp);

    addr = (char *) mmap(NULL, 12288, PROT_READ, MAP_SHARED, fd, 0);

    if (munmap(addr + 8192, 4096) == -1) {
        errExit("munmap");
    }

    char c = addr[4096];

    exit(EXIT_SUCCESS);
}
