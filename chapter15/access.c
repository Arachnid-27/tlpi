#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include "tlpi_hdr.h"

int access2(const char* pathname, int mode) {
    int chk_r = mode & R_OK;
    int chk_w = mode & W_OK;
    int chk_x = mode & X_OK;

    struct stat buf;

    if (stat(pathname, &buf) == -1) {
        errExit("stat %s", pathname);
    }

    mode_t perm = buf.st_mode & 0777;

    if ((geteuid() == buf.st_uid)) {
        if ((chk_r && !(perm & S_IRUSR)) || (chk_w && !(perm & S_IWUSR)) || (chk_x && !(perm & S_IXUSR))) {
            return -1;
        }
        return 0;
    }

    while ((getegid() == buf.st_gid)) { 
        if ((chk_r && !(perm & S_IRGRP)) || (chk_w && !(perm & S_IWGRP)) || (chk_x && !(perm & S_IXGRP))) {
            return -1;
        }
        return 0;
    }

    if ((chk_r && !(perm & S_IROTH)) || (chk_w && !(perm & S_IWOTH)) || (chk_x && !(perm & S_IXOTH))) {
        return -1;
    }

    return 0;
}



int main(int argc, char* argv[]) {
    int flags = O_CREAT | O_RDONLY;

    assert(open("test1", flags, S_IRUSR | S_IWUSR) != -1);
    assert(access2("test1", R_OK) == 0);
    assert(access2("test1", W_OK) == 0);
    assert(access2("test1", X_OK) == -1);
    assert(open("test2", flags, S_IROTH | S_IWOTH | S_IXOTH) != -1);
    assert(access2("test2", R_OK) == -1);
    assert(access2("test2", W_OK) == -1);
    assert(access2("test2", X_OK) == -1);

    printf("Exercise15-4 succeed!\n");

    exit(EXIT_SUCCESS);
}
