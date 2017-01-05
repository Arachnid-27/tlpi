#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdlib.h>

#define MAX_LEN 65535
#define BUF_LEN 4096 

pid_t map[MAX_LEN];

FILE *popen2(const char *command, const char *type) {
    pid_t child;
    char mode;
    int pfd[2];

    if (strcmp(type, "w") == 0) {
        mode = 'w';
    } else if (strcmp(type, "r") == 0) {
        mode = 'r';
    } else {
        return NULL;
    }

    if (pipe(pfd) == -1) {
        return NULL;
    }

    switch (child = fork()) {
    case -1:
        close(pfd[0]);
        close(pfd[1]);
        return NULL;
    case 0:
        if (mode == 'w') {
            if (close(pfd[1]) == -1) {
                _exit(EXIT_FAILURE);
            }
            if (pfd[0] != STDIN_FILENO) {
                if (dup2(pfd[0], STDIN_FILENO) == -1) {
                    _exit(EXIT_FAILURE);
                }
                if (close(pfd[0]) == -1) {
                    _exit(EXIT_FAILURE);
                }
            }
        } else {
            if (close(pfd[0]) == -1) {
                _exit(EXIT_FAILURE);
            }
            if (pfd[1] != STDOUT_FILENO) {
                if (dup2(pfd[1], STDOUT_FILENO) == -1) {
                    _exit(EXIT_FAILURE);
                }
                if (close(pfd[1]) == -1) {
                    _exit(EXIT_FAILURE);
                }
            }
        }

        execl("/bin/sh", "sh", "-c", command, (char*) NULL);
        _exit(127);
    default:
        if (mode == 'w') {
            map[pfd[1]] = child;
            if (close(pfd[0]) == -1) {
                close(pfd[1]);
                return NULL;
            }
        } else {
            map[pfd[0]] = child;
            if (close(pfd[1]) == -1) {
                close(pfd[0]);
                return NULL;
            }
        }
    }

    return fdopen(pfd[mode == 'w'], type);
}

int pclose2(FILE *stream) {
    int fd = fileno(stream);
    if (fd == -1) {
        return -1;
    }

    pid_t child = map[fd];
    while (waitpid(child, NULL, 0) == -1) {
        if (errno != EINTR) {
            return -1;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    char buf[BUF_LEN];

    FILE* fp1 = popen2("ls", "r");
    FILE* fp2 = popen2("wc", "w");

    while (fgets(buf, BUF_LEN, fp1) != NULL) {
        if (fputs(buf, fp2) == -1) {
            exit(EXIT_FAILURE);
        }
    }

    pclose(fp1);
    pclose(fp2);

    exit(EXIT_SUCCESS);
}
