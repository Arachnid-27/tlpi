#include <sys/types.h>
#include <sys/sem.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    int maxind, semid;
    struct semid_ds ds;
    struct seminfo seminfo;

    if ((maxind = semctl(0, 0, SEM_INFO, (struct semid_ds *) &seminfo)) == -1) {
        errExit("semctl-SEM_INFO");
    }

    printf("maxind: %d\n\n", maxind);
    printf("key\t\tsemid\tperm\tnsems\n");

    for (int ind = 0; ind <= maxind; ++ind) {
        if ((semid = semctl(ind, 0, SEM_STAT, &ds)) == -1) {
            if (errno != EINVAL && errno != EACCES) {
                errExit("semctl-SEM_STAT");
            }
        } else {
            printf("0x%08x\t%d\t%o\t%ld\n", ds.sem_perm.__key, semid, ds.sem_perm.mode, (long) ds.sem_nsems);
        }
    }

    exit(EXIT_SUCCESS);
}
