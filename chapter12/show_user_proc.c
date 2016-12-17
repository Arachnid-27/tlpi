#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include "tlpi_hdr.h"
#include "ugid_functions.h"

void split(char *key, char *value, const char *src) {
    while (*src != ':') {
        *key = *src;
        ++key;
        ++src;
    }
    *key = '\0';

    ++src;

    while (*src == ' ' || *src == '\t') ++src;

    while (*src != '\0' && *src != '\n') {
        *value = *src;
        ++value;
        ++src;
    }
    *value = '\0';
}

int is_equal(uid_t uid, const char *str) {
    char buf[33];

    int i = 0;
    while (isdigit(str[i])) {
        buf[i] = str[i];
        ++i;
    }
    buf[i] = '\0';

    return (unsigned) atoi(buf) == uid;
}

void init_path(char *dst, const char *dir) {
    int len = strlen(dir);
    strcpy(dst, "/proc/");
    strcpy(dst + 6, dir);
    *(dst + 6 + len) = '/'; 
    strcpy(dst + 7 + len, "status");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        usageErr("%s user\n", argv[0]);
    }

    uid_t uid = userIdFromName(argv[1]);

    DIR* dir = opendir("/proc");
    FILE* fp;
    char buf[1024];
    char key[256], value[256];
    char name[256];
    struct dirent *dirp;
    int flag;

    while ((dirp = readdir(dir)) != NULL) {
        if (dirp->d_type == DT_DIR && isdigit(dirp->d_name[0])) {
            flag = 0;

            init_path(buf, dirp->d_name);

            if ((fp = fopen(buf, "r")) == NULL) {
                errExit("fopen %s", buf);
            }

            while (!flag && fgets(buf, 1024, fp) != NULL) {
                split(key, value, buf);

                if (strcmp(key, "Name") == 0) {
                    strcpy(name, value);
                } else if (strcmp(key, "Uid") == 0 && is_equal(uid, value)) {
                    printf("Pid=[%s]\tName=[%s]\n", dirp->d_name, name);
                    flag = 1;
                }
            }

            fclose(fp);
        }
    }

    exit(EXIT_SUCCESS);
}
