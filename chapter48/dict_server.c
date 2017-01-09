#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <ctype.h>
#include <fcntl.h>
#include "tlpi_hdr.h"
#include "binary_sems.h"

#define BUF_SIZE 128

#define IPC_PREMS (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)

struct shmseg {
    int key;
    int value;
    int valid;
};

int parse(char* command, int* key, int* value, char *argv) {
    if (argv == 0 || *argv == 0) return -1;

    char buf[BUF_SIZE];
    int offset = 0;

    while (*argv == ' ') ++argv;
    *command = *(argv++);

    while (*argv == ' ') ++argv;
    for (offset = 0; isdigit(argv[offset]); ++offset);

    if (offset == 0) return -1;

    strncpy(buf, argv, offset);
    buf[offset] = 0;
    *key = atoi(buf);
    argv += offset;

    *value = 0;

    if (*argv != 0) {
        while (*argv == ' ') ++argv;
        for (offset = 0; isdigit(argv[offset]); ++offset);

        buf[offset] = 0;
        strncpy(buf, argv, offset);
        *value = atoi(buf);
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int semid, shmid;
    struct shmseg *shmp;
    char buf[BUF_SIZE];

    if (argc != 1 && argc != 3) {
        usageErr("%s [semid shmid]", argv[0]);
    }

    if (argc == 1) {
        if ((semid = semget(IPC_PRIVATE, 1, IPC_CREAT | IPC_PREMS)) == -1) {
            errExit("semget");
        }
        if (initSemAvailable(semid, 0) == -1) {
            errExit("init sem");
        }
        if ((shmid = shmget(IPC_PRIVATE, sizeof(struct shmseg), IPC_CREAT | IPC_PREMS)) == -1) {
            errExit("shmget");
        }
    } else {
        semid = getInt(argv[1], GN_ANY_BASE, "semid");
        shmid = getInt(argv[2], GN_ANY_BASE, "shmid");
    }

    if ((shmp = (struct shmseg *) shmat(shmid, NULL, 0)) == (struct shmseg *) -1) {
        errExit("shmat");
    }

    printf("%d %d\n", semid, shmid);

    printf("usage: command key [value]\n");
    printf("command: [G]et, [I]nsert, [D]elete\n");
    printf("both key and value is integer!\n");

    while (fgets(buf, BUF_SIZE, stdin)) {
        char command;
        int key, value;

        if (parse(&command, &key, &value, buf) == -1) {
            printf("parse error!\n");
        }

        if (reserveSem(semid, 0) == -1) {
            errExit("reserveSem");
        }

        switch (command) {
        case 'G':
            if (shmp[key].valid) {
                printf("get %d, value is %d\n", key, shmp[key].value);
            } else {
                printf("can not find %d\n", key);
            }
            break;
        case 'I':
            if (shmp[key].valid) {
                printf("update %d from %d to %d\n", key, shmp[key].value, value);
            } else {
                shmp[key].valid = 1;
                printf("insert %d, value is %d\n", key, value);
            }
            shmp[key].value = value;
            break;
        case 'D':
            if (shmp[key].valid) {
                shmp[key].valid = 0;
                printf("delete %d, value is %d\n", key, shmp[key].value);
            } else {
                printf("can not find %d\n", key);
            }
            break;
        default:
            printf("unknown command!\n");
        }

        if (releaseSem(semid, 0) == -1) {
            errExit("releaseSem");
        }
    }

    if (argc == 1) {
        semctl(semid, 0, IPC_RMID);
        shmctl(shmid, IPC_RMID, 0);
    }

    shmdt(shmp);

    exit(EXIT_SUCCESS);
}
