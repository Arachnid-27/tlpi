#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include "tlpi_hdr.h"
#include "ugid_functions.h"

#define MAX_PROC 16348 

struct proc {
    int pid;
    char name[256];
    struct proc *sub;
    struct proc *next;
};

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

void init_path(char *dst, const char *dir) {
    int len = strlen(dir);
    strcpy(dst, "/proc/");
    strcpy(dst + 6, dir);
    *(dst + 6 + len) = '/'; 
    strcpy(dst + 7 + len, "status");
}

struct proc *create_proc(struct proc **list, int pid) {
    struct proc *p = (struct proc *) malloc(sizeof(struct proc));
    p->pid = pid;
    p->next = NULL;
    p->sub = NULL;
    list[pid] = p;
    return p;
}

void dfs(struct proc *p, int depth) {
    for (int i = 0; i < 4 * (depth - 1); ++i) {
        putchar(' ');
    }

    if (depth > 0) {
        putchar('|');
        for (int i = 0; i < 3; ++i) {
            putchar('-');
        }
    }

    printf("[%d]%s\n", p->pid, p->name);

    if (p->sub != NULL) {
        dfs(p->sub, depth + 1);
        p = p->sub;
        while (p->next != NULL) {
            dfs(p->next, depth + 1);
            p = p->next;
        }
    }
}

int main(int argc, char *argv[]) {
    struct proc *list[MAX_PROC];
    DIR* dir = opendir("/proc");
    FILE* fp;
    char buf[1024];
    char key[256], value[256];
    struct dirent *dirp;
    int flag;

    strcpy(create_proc(list, 0)->name, "init");

    while ((dirp = readdir(dir)) != NULL) {
        if (dirp->d_type == DT_DIR && isdigit(dirp->d_name[0])) {
            flag = 0;

            init_path(buf, dirp->d_name);

            if ((fp = fopen(buf, "r")) == NULL) {
                errExit("fopen %s", buf);
            }

            struct proc *p = create_proc(list, atoi(dirp->d_name));

            while (!flag && fgets(buf, 1024, fp) != NULL) {
                split(key, value, buf);

                if (strcmp(key, "Name") == 0) {
                    strcpy(p->name, value);
                } else if (strcmp(key, "PPid") == 0) {
                    struct proc *t = list[atoi(value)];
                    if (t->sub == NULL) {
                        t->sub = p;
                    } else {
                        t = t->sub;
                        while (t->next != NULL) {
                            t = t->next;
                        }
                        t->next = p;
                    }
                    flag = 1;
                }
            }

            fclose(fp);
        }
    }

    dfs(list[0], 0);

    exit(EXIT_SUCCESS);
}
