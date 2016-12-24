#include <signal.h>
#include <libgen.h>
#include "tlpi_hdr.h"

#define CMD_SIZE 200

int flag;

static void handler(int sig) {
    flag = 1;
}

int main(int argc, char *argv[]) {
    char cmd[CMD_SIZE];
    pid_t childPid;
    sigset_t blockMask, origMask, emptyMask;
    struct sigaction sa;

    setbuf(stdout, NULL);

    sigemptyset(&blockMask);
    sigaddset(&blockMask, SIGCHLD);
    if (sigprocmask(SIG_BLOCK, &blockMask, &origMask) == -1) {
        errExit("sigprocmask");
    }

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDSTOP;
    sa.sa_handler = handler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        errExit("sigaction");
    }

    printf("Parent PID=%ld\n", (long) getpid());

    switch (childPid = fork()) {
    case -1:
        errExit("fork");
    case 0:
        printf("Child (PID=%ld) exiting\n", (long) getpid());

        _exit(EXIT_SUCCESS);
    default:
        sigemptyset(&emptyMask);
        if (sigsuspend(&emptyMask) == -1 && errno != EINTR) {
            errExit("sigsuspend");
        }

        if (flag == 1) {
            snprintf(cmd, CMD_SIZE, "ps | grep %s", basename(argv[0]));
            system(cmd);

            if (kill(childPid, SIGKILL) == -1)
                errMsg("kill");
            sleep(2);

            printf("After sending SIGKILL to zombie (PID=%ld):\n", (long) childPid);
            system(cmd);
        }

        if (sigprocmask(SIG_SETMASK, &origMask, NULL) == -1) {
            errExit("sigprocmask");
        }

        exit(EXIT_SUCCESS);
    }
}
