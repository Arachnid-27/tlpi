#define _GNU_SOURCE
#include <sched.h>
#include <sys/times.h>
#include "tlpi_hdr.h"

void loop(long unit) {
    struct tms ticks;
    int count = 0;
    float sec = 0.0;
    float point = 0.25;

    while (count < 12) {
        times(&ticks);
        sec = (ticks.tms_stime + ticks.tms_utime) * 1.0 / unit;
        if (sec > point) {
            ++count;
            printf("[%02d] pid=%d time=%0.2f\n", count, getpid(), sec);
            if (count % 4 == 0) {
                printf("pid=%d sched_yield\n", getpid());
                sched_yield();
            }
            point += 0.25;
        }
    }
}

int main(int argc, char *argv[]) {
    cpu_set_t set;

    CPU_ZERO(&set);
    CPU_SET(0, &set);

    if (sched_setaffinity(0, sizeof(cpu_set_t), &set) == -1) {
        errExit("sched_setaffinity");
    }

    struct sched_param param;

    param.sched_priority = 1;

    if (sched_setscheduler(0, SCHED_FIFO, &param) == -1) {
        errExit("sched_setscheduler");
    }

    long unit = sysconf(_SC_CLK_TCK);

    switch (fork()) {
    case -1:
        errExit("fork");
    case 0:
        loop(unit);
        _exit(EXIT_SUCCESS);
    default:
        loop(unit);
    }

    exit(EXIT_SUCCESS);
}

