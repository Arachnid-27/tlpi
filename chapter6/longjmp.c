#include <setjmp.h>
#include "tlpi_hdr.h"

jmp_buf env;

void f2() {
    longjmp(env, 2);
}

void f1(int argc) {
    if (argc == 1) {
        longjmp(env, 1);
    }
    f2();
}

int main(int argc, char *argv[]) {
    switch(setjmp(env)) {
        case 0:
            printf("Calling f1() after initial setjmp()\n");
            f1(argc);
            break;
        case 1:
            printf("We jjumped back from f1()\n");
            break;
        case 2:
            printf("We jjumped back from f2()\n");
            break;
    }
    exit(EXIT_SUCCESS);
}
