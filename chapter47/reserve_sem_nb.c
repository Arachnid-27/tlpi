#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/sem.h>
#include <sys/types.h>

int reserve_sem_nb(int sem_id, int sem_num) {
    struct sembuf sop;

    sop.sem_num = sem_num;
    sop.sem_op = -1;
    sop.sem_flg = IPC_NOWAIT;

    return semop(sem_id, &sop, 1);
}

int main(int argc, char *argv[]) {
    int semid;

    assert((semid = semget(IPC_PRIVATE, 1, S_IRUSR | S_IWUSR)) != -1);
    assert(reserve_sem_nb(semid, 0) == -1);
    assert(errno == EAGAIN);

    printf("Exercise47-4 succeed!\n");

    exit(EXIT_SUCCESS);
}
