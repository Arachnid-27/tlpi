#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
    struct stat st;
    key_t key = ftok("ftok", 16);

    assert(stat("ftok", &st) == 0);
    assert(key == (((16 & 0xFF) << 24) | ((minor(st.st_dev) & 0xFF) << 16) | (st.st_ino & 0xFFFF)));

    printf("Exercise45-1 succeed!\n");

    exit(EXIT_SUCCESS);
}
