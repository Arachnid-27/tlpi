#include <syslog.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usageErr("%s [-l level] message\n", argv[0]);
    }

    int priority = LOG_USER;

    if (getopt(argc, argv, "l:") != -1) {
        switch (getInt(optarg, GN_ANY_BASE, "level")) {
        default:
        case 0:
            priority |= LOG_DEBUG;
            break;
        case 1:
            priority |= LOG_INFO;
            break;
        case 2:
            priority |= LOG_NOTICE;
            break;
        case 3:
            priority |= LOG_WARNING;
            break;
        case 4:
            priority |= LOG_ERR;
            break;
        case 5:
            priority |= LOG_CRIT;
            break;
        case 6:
            priority |= LOG_ALERT;
            break;
        case 7:
            priority |= LOG_EMERG;
            break;
        }
    }

    openlog(argv[0], LOG_CONS, LOG_USER);
    syslog(priority, "%s", argv[optind]);

    exit(EXIT_SUCCESS);
}
