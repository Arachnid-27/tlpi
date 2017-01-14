#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("%s: command\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int cfd, len;
    struct sockaddr_in server_addr;
    char buf[1024];

    if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        printf("socket error!\n");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7777);

    if (connect(cfd, (struct sockaddr *) &server_addr, sizeof(struct sockaddr_in)) == -1) {
        printf("connect error!\n");
        exit(EXIT_FAILURE);
    }

    len = strlen(argv[1]) + 1;
    if (write(cfd, argv[1], len) != len) {
        printf("write error!\n");
        exit(EXIT_FAILURE);
    }

    while (read(cfd, buf, 1024) > 0) {
        printf("%s", buf);
    }

    exit(EXIT_SUCCESS);
}

