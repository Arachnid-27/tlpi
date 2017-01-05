#include <sys/types.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include "curr_time.h"
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    struct sembuf sop;
    int semid;

    setbuf(stdout, NULL);

    if ((semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR)) == -1) {
        errExit("semget");
    }

    sop.sem_num = 0;
    sop.sem_flg = 0;

    switch (fork()) {
    case -1:
        errExit("fork");
    case 0:
        printf("[%s %ld] Child started - doing some work\n", currTime("%T"), (long) getpid());

        sleep(2);

        printf("[%s %ld] Child about to signal parent\n", currTime("%T"), (long) getpid());

        sop.sem_op = 1;

        if (semop(semid, &sop, 1) == -1) {
            errExit("semop");
        }

        _exit(EXIT_SUCCESS);
    default:
        printf("[%s %ld] Parent about to wait for signal\n", currTime("%T"), (long) getpid());

        sop.sem_op = -1;

        if (semop(semid, &sop, 1) == -1) {
            errExit("semop");
        }

        printf("[%s %ld] Parent got signal\n", currTime("%T"), (long) getpid());
    }

    exit(EXIT_SUCCESS);
}
