#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        usageErr("%s src dst\n", argv[0]);
    }

    int fd1, fd2;
    char *src, *dst;
    struct stat st;

    if ((fd1 = open(argv[1], O_RDONLY)) == -1) {
        errExit("open %s", argv[1]);
    }

    if (fstat(fd1, &st) == -1) {
        errExit("fstat");
    }

    if ((fd2 = open(argv[2], O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR)) == -1) {
        errExit("open %s", argv[2]);
    }

    if (ftruncate(fd2, st.st_size) == -1) {
        errExit("ftruncate");
    }

    src = (char *) mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd1, 0);
    dst = (char *) mmap(NULL, st.st_size, PROT_WRITE, MAP_SHARED, fd2, 0);

    memcpy(dst, src, st.st_size);

    exit(EXIT_SUCCESS);
}
