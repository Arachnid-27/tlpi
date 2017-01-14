#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static void child_handler(int sig) {
    int saved;

    saved = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved;
}

int main(int argc, char *argv[]) {
    int sfd, cfd, optval;
    struct sockaddr_in server_addr;
    struct sigaction sa;
    char buf[1024];

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sa.sa_handler = child_handler;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        printf("sigaction SIGCHLD error!\n");
        exit(EXIT_FAILURE);
    }

    if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("socket error!\n");
        exit(EXIT_FAILURE);
    }

    optval = 1;
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        printf("setsockopt error!\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7777);

    if (bind(sfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) == -1) {
        printf("bind error!\n");
        exit(EXIT_FAILURE);
    }

    if (listen(sfd, 10) == -1) {
        printf("listen error!\n");
        exit(EXIT_FAILURE);
    }

    while ((cfd = accept(sfd, NULL, NULL)) != -1) {
        switch (fork()) {
        case -1:
            printf("fork error!\n");
            exit(EXIT_FAILURE);
        case 0:
            close(sfd);

            if (read(cfd, buf, 1024) == -1) {
                printf("read error!\n");
                _exit(EXIT_FAILURE);
            }

            if (dup2(1, cfd) == -1 || dup2(2, cfd) == -1) {
                printf("dup2 error!\n");
                _exit(EXIT_FAILURE);
            }

            system(buf);

            _exit(EXIT_SUCCESS);
        default:
            close(cfd);
        }
    }

    exit(EXIT_SUCCESS);
}
