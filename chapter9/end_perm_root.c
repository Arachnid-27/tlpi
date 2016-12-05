#define _GNU_SOURCE
#include <unistd.h>
#include <assert.h>
#include "tlpi_hdr.h"

void show() {
    uid_t ruid, euid, suid;
   
    if (getresuid(&ruid, &euid, &suid) == -1) {
        errExit("getresuid");
    }

    printf("ruid=%ld euid=%ld suid=%ld\n", (long) ruid, (long) euid, (long) suid);
}

void check(uid_t value) {
    uid_t ruid, euid, suid;
   
    if (getresuid(&ruid, &euid, &suid) == -1) {
        errExit("getresuid");
    }

    assert(ruid == value);
    assert(euid == value);
    assert(suid == value);
}

int main(int argc, char *argv[]) {
    uid_t ruid = getuid();

    printf("--------------------------------\n");

    printf("init:\n");
    show();

    printf("--------------------------------\n");

    printf("after setuid(ruid):\n");
    setuid(ruid);
    show();
    check(ruid);

    printf("--------------------------------\n");

    printf("seteuid impossible!\n");

    printf("--------------------------------\n");

    printf("after setreuid(ruid, ruid):\n");
    setreuid(ruid, ruid);
    show();
    check(ruid);

    printf("--------------------------------\n");

    printf("after setresuid(-1, ruid, ruid):\n");
    setresuid(-1, ruid, ruid);
    show();
    check(ruid);

    printf("--------------------------------\n");

    printf("Exercise9-5 (b) succeed!\n");
    exit(EXIT_SUCCESS);
}

