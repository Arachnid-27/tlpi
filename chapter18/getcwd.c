#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include "tlpi_hdr.h"

char *getcwd2(char *buf, size_t size) {
    int fd;

    if ((fd = open(".", O_RDONLY)) == -1) {
        fchdir(fd);
        return NULL;
    }

    struct stat st;
    struct dirent *dirp;
    char saved[256][256];
    int i = 0;
    dev_t dev;
    ino_t ino;

    while (1) {
        if (stat(".", &st) == -1) {
            fchdir(fd);
            return NULL;
        }

        if (dev == st.st_dev && ino == st.st_ino) {
            break;
        }

        dev = st.st_dev;
        ino = st.st_ino;

        chdir("..");

        DIR* dir = opendir(".");

        while ((dirp = readdir(dir)) != NULL) {
            if (strcmp(dirp->d_name, ".") || strcmp(dirp->d_name, "..")) {
                if (stat(dirp->d_name, &st) == -1) {
                    fchdir(fd);
                    return NULL;
                }
                if (dev == st.st_dev && ino == st.st_ino) {
                    strcpy(saved[i++], dirp->d_name);
                    break;
                }
            }
        }

        if (stat("..", &st) == -1) {
            fchdir(fd);
            return NULL;
        }

        dev = st.st_dev;
        ino = st.st_ino;
    }

    buf[0] = '/';

    size_t count = 1;
    while (--i >= 0) {
        if (count >= size) {
            fchdir(fd);
            return NULL;
        }

        strcpy(buf + count, saved[i]);
        count += strlen(saved[i]);

        if (count >= size) {
            fchdir(fd);
            return NULL;
        }

        buf[count++] = '/';
    }
    buf[count - 1] = '\0';

    fchdir(fd);
    return buf;
}

int main(int argc, char* argv[]) {
    char buf1[1024], buf2[1024];

    assert(getcwd(buf1, 1024) != NULL);
    assert(getcwd2(buf2, 1024) != NULL);
    assert(strcmp(buf1, buf2) == 0);
    assert(getcwd(buf1, 5) == NULL);
    assert(getcwd2(buf2, 5) == NULL);

    printf("Exercise18-5 succeed!\n");
    
    exit(EXIT_SUCCESS);
}
