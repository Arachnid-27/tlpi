#include "svmsg_file.h"

int server_id;

static void clear() {
    if (msgctl(server_id, IPC_RMID, NULL) == -1) {
        syslog(LOG_WARNING, "msgctl IPC_RMID %d error", server_id);
    }
    unlink(SERVER_PATH);
}

static void child_handler(int sig) {
    int savedErrno;

    savedErrno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = savedErrno;
}

static void exit_handler(int sig) {
    syslog(LOG_INFO, "kill by SIGTERM or SIGINT");
    clear();
    signal(sig, SIG_DFL);
    raise(sig);
}

static void alarm_handler(int sig) {
}

static void serve_request(const struct requestMsg *req) {
    int fd;
    int saved;
    ssize_t num_read;
    ssize_t num_send;
    struct responseMsg resp;
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = alarm_handler;
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        syslog(LOG_ERR, "sigaction SIGALRM error");
        _exit(EXIT_FAILURE);
    }

    if ((fd = open(req->pathname, O_RDONLY)) == -1) {
        resp.mtype = RESP_MT_FAILURE;
        snprintf(resp.data, sizeof(resp.data), "%s", "Couldn't open");
        msgsnd(req->clientId, &resp, strlen(resp.data) + 1, 0);
        _exit(EXIT_FAILURE);
    }

    resp.mtype = RESP_MT_DATA;
    while ((num_read = read(fd, resp.data, RESP_MSG_SIZE)) > 0) {
        alarm(2);

        num_send = msgsnd(req->clientId, &resp, num_read, 0);

        saved = errno;
        alarm(0);
        errno = saved;

        if (num_send == -1) {
            if (errno == EINTR) {
                syslog(LOG_WARNING, "maybe client is closed");
                if (msgctl(server_id, IPC_RMID, NULL) == -1) {
                    syslog(LOG_WARNING, "msgctl IPC_RMID %d error", req->clientId);
                }
                _exit(EXIT_SUCCESS);
            }
            syslog(LOG_ERR, "send %s error", req->pathname);
            break;
        }
    }

    if (num_read == -1) {
        syslog(LOG_ERR, "read %s error", req->pathname);
    }

    resp.mtype = RESP_MT_END;
    msgsnd(req->clientId, &resp, 0, 0);
}

int main(int argc, char *argv[]) {
    struct requestMsg req;
    ssize_t num_rcv;
    int fd;
    struct sigaction sa;

    openlog(argv[0], LOG_PID | LOG_CONS, LOG_USER);

    if (daemon(0, 0) == -1) {
        syslog(LOG_ERR, "become daemon error");
        exit(EXIT_FAILURE);
    }

    if (atexit(clear) != 0) {
        syslog(LOG_ERR, "atexit error");
        exit(EXIT_FAILURE);
    }

    server_id = msgget(IPC_PRIVATE, S_IRUSR | S_IWUSR);

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0; 
    sa.sa_handler = exit_handler;
    if (sigaction(SIGTERM, &sa, NULL) == -1 || sigaction(SIGINT, &sa, NULL) == -1) {
        syslog(LOG_ERR, "sigaction SIGTERM or SIGINT error");
        exit(EXIT_FAILURE);
    }

    if ((fd = open(SERVER_PATH, O_CREAT | O_EXCL | O_WRONLY, S_IRUSR | S_IWUSR)) == -1) {
        syslog(LOG_ERR, "open %s error", SERVER_PATH);
        exit(EXIT_FAILURE);
    }

    if (write(fd, &server_id, sizeof(int)) != sizeof(int)) {
        syslog(LOG_ERR, "write server_id error");
        exit(EXIT_FAILURE);
    }

    sa.sa_flags = SA_RESTART;
    sa.sa_handler = child_handler;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        syslog(LOG_ERR, "sigaction SIGCHLD error");
        exit(EXIT_FAILURE);
    }

    for (;;) {
        num_rcv = msgrcv(server_id, &req, REQ_MSG_SIZE, 0, 0);
        if (num_rcv == -1) {
            if (errno == EINTR) {       /* Interrupted by SIGCHLD handler? */
                continue;               /* ... then restart msgrcv() */
            }
            syslog(LOG_ERR, "msgrcv error");
            break;                      /* ... so terminate loop */
        }

        switch (fork()) {
        case -1:
            syslog(LOG_ERR, "fork error");
            break;
        case 0:
            serve_request(&req);
            _exit(EXIT_SUCCESS);
        }
    }

    exit(EXIT_SUCCESS);
}
