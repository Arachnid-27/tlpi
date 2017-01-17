#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    int stfd, sufd, ctfd, nfds;
    int count;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    fd_set rfds;
    char buf[1024];

    stfd = socket(AF_INET, SOCK_STREAM, 0);
    sufd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7777);

    if (bind(stfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) == -1) {
        printf("bind error\n");
        exit(EXIT_FAILURE);
    }

    if (listen(stfd, 5) == -1) {
        printf("listen error\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7777);

    if(bind(sufd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) == -1) {
        printf("bind error\n");
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&rfds);
    FD_SET(stfd, &rfds);
    FD_SET(sufd, &rfds);

    nfds = sufd + 1;

    while (select(nfds, &rfds, NULL, NULL, NULL) > 0) {
        if (FD_ISSET(stfd, &rfds)) {
            ctfd = accept(stfd, NULL, NULL);
            if ((count = read(ctfd, buf, 1024)) == -1) {
                printf("read error\n");
            } else if (write(ctfd, buf, count) != count) {
                printf("write error\n");
            }
            close(ctfd);
        }

        if (FD_ISSET(sufd, &rfds)) {
            if ((count = recvfrom(sufd, buf, 1024, 0, (struct sockaddr *) &client_addr, &client_len)) == -1) {
                printf("read error\n");
            } else if (sendto(sufd, buf, count, 0, (struct sockaddr *) &client_addr, client_len) != count) {
                printf("write error\n");
            }
        }

        FD_ZERO(&rfds);
        FD_SET(stfd, &rfds);
        FD_SET(sufd, &rfds);
    }

    exit(EXIT_SUCCESS);
}
