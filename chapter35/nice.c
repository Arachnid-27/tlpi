#include <errno.h>
#include <sys/resource.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usageErr("%s [-n adjust] command\n", argv[0]);
    }

    int origin, adjust;

    if (getopt(argc, argv, "n:") != -1) {
        adjust = getInt(optarg, GN_ANY_BASE, "adjust");
        errno = 0;
        if ((origin = getpriority(PRIO_PROCESS, 0)) == -1 && errno != 0) {
            errExit("getpriority");
        }
        if (setpriority(PRIO_PROCESS, 0, origin + adjust) == -1) {
            errExit("setpriority");
        }
    }

    execvp(argv[optind], argv + optind);

    errExit("execvp");
}
