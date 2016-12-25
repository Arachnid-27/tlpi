#include <sys/inotify.h>
#include <limits.h>
#include <ftw.h>
#include "tlpi_hdr.h"

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))
#define MAX_NOTIFY 512

size_t inotifyNum;
int inotifyFd;
char *inotifyWd[MAX_NOTIFY];
int wdNum;

static void getFullPath(char *dst, int wd, const char *name) {
    char *parent = inotifyWd[wd];

    strcpy(dst, parent);
    strcat(dst, "/");
    strcat(dst, name);
}

static int getWdByName(const char *name) {
    for (int i = 1; i <= wdNum; ++i) {
        if (strcmp(inotifyWd[i], name) == 0) {
            return i;
        }
    }
    return -1;
}

static void removeInotifyWatch(int wd) {
    printf("remove Watching wd %d %s\n", wd, inotifyWd[wd]);
    free(inotifyWd[wd]);
}

static void addInotifyWatch(const char *pathname) {
    uint32_t mask = IN_CREATE | IN_DELETE | IN_MOVE;

    int wd = inotify_add_watch(inotifyFd, pathname, mask); 
    if (wd == -1) {
        errExit("inotify_add_watch %s", pathname);
    }

    inotifyWd[wd] = (char *) malloc(strlen(pathname) + 1);
    strcpy(inotifyWd[wd], pathname);

    printf("Watching %s using wd %d\n", pathname, wd);
    if (wd > wdNum) wdNum = wd;
}

static int dirTree(const char *pathname, const struct stat *sbuf, int type, struct FTW *ftwb) {
    if (type == FTW_D) {
        addInotifyWatch(pathname);
    }
    return 0;
}

static void displayInotifyEvent(struct inotify_event *event) {
    if (event->mask & IN_CREATE) {
        if (event->mask & IN_ISDIR) {
            printf("%03ld: create directory %s in %s\n", ++inotifyNum, event->name, inotifyWd[event->wd]);
            char buf[256];
            getFullPath(buf, event->wd, event->name);
            addInotifyWatch(buf);
        } else {
            printf("%03ld: create file %s in %s\n", ++inotifyNum, event->name, inotifyWd[event->wd]);
        }
    }

    if (event->mask & IN_MOVED_FROM) {
        printf("%03ld: moving %s\n", ++inotifyNum, event->name);
        if (event->mask & IN_ISDIR) {
            char buf[256];
            getFullPath(buf, event->wd, event->name);
            int wd = getWdByName(buf);
            if (wd != -1) {
                inotify_rm_watch(inotifyFd, wd);
            }
        }
    }

    if (event->mask & IN_IGNORED) {
        removeInotifyWatch(event->wd);
    }

    if (event->mask & IN_MOVED_TO) {
        printf("%03ld: get %s\n", ++inotifyNum, event->name);
        if (event->mask & IN_ISDIR) {
            char buf[256];
            getFullPath(buf, event->wd, event->name);
            addInotifyWatch(buf);
        }
    }

    if (event->mask & IN_DELETE) {
        if (event->mask & IN_ISDIR) {
            printf("%03ld: delete directory %s in %s\n", ++inotifyNum, event->name, inotifyWd[event->wd]);
        } else {
            printf("%03ld: delete file %s in %s\n", ++inotifyNum, event->name, inotifyWd[event->wd]);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usageErr("%s dir\n", argv[0]);
    }

    if ((inotifyFd = inotify_init()) == -1) {
        errExit("inotify_init");
    }

    int flags = FTW_MOUNT | FTW_PHYS;

    if (nftw(argv[1], dirTree, 20, flags) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }

    char buf[BUF_LEN] __attribute__((aligned(8)));
    ssize_t count;
    struct inotify_event *event;

    while (1) {
        count = read(inotifyFd, buf, BUF_LEN);

        if (count == 0) {
            fatal("read from inotify fd returned 0!");
        }

        if (count == -1) {
            errExit("read");
        }

        for (char *p = buf; p < buf + count;) {
            event = (struct inotify_event *) p;
            displayInotifyEvent(event);
            p += (sizeof(struct inotify_event) + event->len);
        }
    }

    exit(EXIT_SUCCESS);
}
