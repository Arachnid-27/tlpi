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

int main(int argc, char *argv[]) {
    uid_t ruid, suid;

    ruid = getuid();
    suid = geteuid();

    printf("--------------------------------\n");

    printf("init:\n");
    show();

    printf("--------------------------------\n");

    printf("setuid impossible!\n");

    printf("--------------------------------\n");

    printf("after seteuid(ruid):\n");
    seteuid(ruid);
    show();
    assert(geteuid() == ruid);

    printf("after seteuid(suid):\n");
    seteuid(suid);
    show();
    assert(geteuid() == suid);

    printf("--------------------------------\n");

    printf("after setreuid(suid, ruid):\n");
    setreuid(suid, ruid);
    show();
    assert(geteuid() == ruid);

    printf("after setreuid(ruid, suid):\n");
    setreuid(ruid, suid);
    show();
    assert(geteuid() == suid);

    printf("--------------------------------\n");

    printf("after setresuid(-1, ruid, -1):\n");
    setresuid(-1, ruid, -1);
    show();
    assert(geteuid() == ruid);

    printf("after setresuid(-1, suid, -1):\n");
    setresuid(-1, suid, -1);
    show();
    assert(geteuid() == suid);

    printf("--------------------------------\n");

    printf("Exercise9-5 (a) succeed!\n");
    exit(EXIT_SUCCESS);
}
