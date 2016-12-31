#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/times.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usageErr("%s command arg...\n", argv[0]);
    }

    struct rusage ru;
    pid_t pid;

    switch (pid = fork()) {
    case -1:
        errExit("fork");
    case 0:
        execvp(argv[1], argv + 1);
        _exit(EXIT_FAILURE);
    default:
        waitpid(pid, NULL, 0);
        if (getrusage(RUSAGE_CHILDREN, &ru) == -1) {
            errExit("getrusage");
        }
        printf("user: %0.3fs\n", (ru.ru_utime.tv_sec + ru.ru_utime.tv_usec * 1.0 / 1e6));
        printf("sys: %0.3fs\n", (ru.ru_stime.tv_sec + ru.ru_stime.tv_usec * 1.0 / 1e6));
    }

    exit(EXIT_SUCCESS);
}
