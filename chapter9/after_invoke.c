#define _GNU_SOURCE
#include <unistd.h>
#include <sys/fsuid.h>
#include "tlpi_hdr.h"

void show() {
    uid_t ruid, euid, suid, fsuid;
   
    if (getresuid(&ruid, &euid, &suid) == -1) {
        errExit("getresuid");
    }

    fsuid = setfsuid(0);

    printf("ruid=%ld euid=%ld suid=%ld fsuid=%ld\n", (long) ruid, (long) euid, (long) suid, (long) fsuid);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usageErr("%s [abcde]\n", argv[0]);
    }

    show();

    switch(argv[1][0]) {
        case 'a':
            setuid(2000);
            break;
        case 'b':
            setreuid(-1, 1000);
            break;
        case 'c':
            seteuid(2000);
            break;
        case 'd':
            setfsuid(2000);
            break;
        case 'e':
            setresuid(-1, 2000, 3000);
            break;
    }

    show();

    exit(EXIT_SUCCESS);
}
